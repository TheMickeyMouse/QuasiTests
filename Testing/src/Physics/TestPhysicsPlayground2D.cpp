#include "TestPhysicsPlayground2D.h"

#include <imgui.h>

#include "Extension/ImGuiExt.h"
#include "Meshes/Circle.h"
#include "Meshes/Stadium.h"

namespace Test {
    void TestPhysicsPlayground2D::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));

        world = { { 0, -40.0f }, Physics2D::Drag(0.98f, 60.0f) };
        scene.SetProjection(Math::Matrix3D::ortho_projection({ -40, 40, -30, 30, -1, 1 }));

        world.CreateBody<Physics2D::EdgeShape>(
            { { 0, -30 }, Physics2D::BodyType::STATIC, 0.0f },
            Math::fVector2 { -100, 0 }, Math::fVector2 { 100, 0 }, 5); // floor
        AddBodyTint(Math::fColor::BETTER_GRAY());

        world.CreateBody<Physics2D::CircleShape>({ { 0, 20 }, Physics2D::BodyType::DYNAMIC, 1.0f }, 5);
        AddBodyTint(Math::fColor::BETTER_BLUE());
    }

    void TestPhysicsPlayground2D::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        const auto& imguiio = ImGui::GetIO();
        if (!imguiio.WantCaptureMouse && !imguiio.WantCaptureKeyboard) {
            const auto& mouse = gdevice.GetIO().Mouse;
            const Math::fVector2
                rawMouse = mouse.GetMousePos().map({ -1, 1, 1, -1 }, { -40, 40, -30, 30 }),
                mousePos = rawMouse * zoomFactor + cameraPosition;

            if (mouse.LeftOnPress()) {
                selectedControl = ~0;
                for (u32 i = 0; i < controlPointCount; ++i) {
                    if (Physics2D::Collision::OverlapShapeDyn(
                        Physics2D::CircleShape { 0.0f }, mousePos,
                        Physics2D::CircleShape { 2.0f }, Selected()->body->position + controlPoints[i])) {
                        selectedControl = i;
                    }
                }
                if (selectedControl == ~0) {
                    if (const auto s = FindAt(mousePos); s != ~0) {
                        Select(s);
                        selectOffset = Selected()->body->position - mousePos;
                    } else Unselect();
                }
            }
            if (mouse.LeftPressed()) {
                if (selectedControl != ~0) {
                    controlPoints[selectedControl] = mousePos - Selected()->body->position;
                    UpdateBodyFromControl();
                } else if (selectedIndex != ~0) {
                    Selected()->body->position = mousePos + selectOffset;
                }
            }

            if (mouse.MiddleOnPress()) lastDragPosition = rawMouse;
            if (mouse.MiddlePressed()) {
                cameraPosition += (lastDragPosition - rawMouse) * zoomFactor;
                lastDragPosition = rawMouse;
            }

            if (mouse.RightOnPress()) {
                const u32 target = FindAt(mousePos);
                if (target != ~0 && bodyData[target].body->IsDynamic()) {
                    Select(target);
                }
            }

            if (mouse.RightPressed() && selectedIndex != ~0) {
                hasAddedForce = true;
            }

            if (mouse.RightOnRelease() && selectedIndex != ~0 && !selectedIsStatic) {
                const bool scale = gdevice.GetIO().Keyboard.KeyPressed(IO::Key::LCONTROL);
                originalVelocity -= (scale ? 10.0f : 1.0f) * (mousePos - Selected()->body->position);
                Unselect();
                hasAddedForce = false;
            }

            zoomFactor *= (float)(1 - 0.05f * mouse.GetMouseScrollDelta().y);
        }

        worldUpdate:
        if (onPause & 1) return;
        world.Update(std::min(deltaTime, 1 / 60.0f), 8, 2);
        onPause = onPause == 2 ? 1 : 0;
    }

    void TestPhysicsPlayground2D::OnRender(Graphics::GraphicsDevice& gdevice) {
        worldMesh.Clear();

        u32 i = 0;
        using namespace Graphics::VertexBuilder;
        for (const auto& [body, color] : bodyData) {
            switch (body->shape->TypeIndex()) {
                case Physics2D::CIRCLE: {
                    const Ref<const Physics2D::CircleShape> circle = body->shape.As<Physics2D::CircleShape>();
                    Graphics::MeshUtils::CircleCreator::Merge(
                        { 16 },
                        Vertex::Blueprint {
                            .Position = FromArg<PositionArg2D>([&] (const Math::fVector2 vec) {
                                return vec * circle->radius + body->position;
                            }),
                            .Color = Constant { color }
                        },
                        worldMesh
                    );
                    break;
                }
                case Physics2D::EDGE: {
                    Ref<const Physics2D::EdgeShape> edge = body->shape.As<Physics2D::EdgeShape>();
                    Graphics::MeshUtils::StadiumCreator::Merge(
                        { .start = edge->start, .end = edge->end, .radius = edge->radius, .subdivisions = 4 },
                        Vertex::Blueprint {
                            .Position = FromArg<PositionArg2D>([&] (const Math::fVector2 vec) { return vec + body->position; }),
                            .Color = Constant { color }
                        }, worldMesh
                    );
                    break;
                }
                case Physics2D::TRIANGLE: {
                    Ref<const Physics2D::TriangleShape> tri = body->shape.As<Physics2D::TriangleShape>();
                    auto meshp = worldMesh.NewBatch();
                    meshp.PushV({ body->position + tri->a, color });
                    meshp.PushV({ body->position + tri->b, color });
                    meshp.PushV({ body->position + tri->c, color });
                    meshp.PushI(0, 1, 2);
                    break;
                }
                case Physics2D::MAX:
                    break;
            }
            ++i;
        }

        if (hasAddedForce) {
            const Math::fColor blue = Math::fColor::BLUE();
            const Math::fVector2 mouse = gdevice.GetIO().Mouse.GetMousePos().map({ -1, 1, 1, -1 }, { -40, 40, -30, 30 }) * zoomFactor + cameraPosition,
                                 direction = (Selected()->body->position - mouse).norm(0.5f);
            auto meshp = worldMesh.NewBatch();
            meshp.PushV({ Selected()->body->position + Math::fVector2 { direction.y, -direction.x }, blue });
            meshp.PushV({ Selected()->body->position + Math::fVector2 { -direction.y, direction.x }, blue });
            meshp.PushV({ mouse, blue });
            meshp.PushI(0, 1, 2);
        }

        if (selectedIndex != ~0) {
            AddNewPoint(Selected()->body->position, Math::fColor::RED());
        }

        if (controlPointCount) {
            for (u32 j = 0; j < controlPointCount; ++j)
                AddNewPoint(controlPoints[j] + Selected()->body->position, Math::fColor::GREEN());
        }

        Math::fRect3D viewport = Math::fRect3D { -40, 40, -30, 30, -1, 1 } * zoomFactor + cameraPosition;
        viewport.min.z = -1;
        viewport.max.z = +1;
        scene.Draw(worldMesh, Graphics::UseArgs({{ "u_projection", Math::Matrix3D::ortho_projection(viewport) }}, false));
    }

    void TestPhysicsPlayground2D::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (ImGui::Button("+ New Circle")) {
            world.CreateBody<Physics2D::CircleShape>({ 0, Physics2D::BodyType::DYNAMIC, 1.0f }, 5.0f);
            goto addColor;
        }
        if (ImGui::Button("+ New Edge")) {
            world.CreateBody<Physics2D::EdgeShape>({ 0, Physics2D::BodyType::DYNAMIC, 1.0f },
                Math::fVector2 { 0, 5 }, Math::fVector2 { 0, 0 }, 5.0f);
            goto addColor;
        }
        if (ImGui::Button("+ New Triangle")) {
            world.CreateBody<Physics2D::TriangleShape>({ 0, Physics2D::BodyType::DYNAMIC, 1.0f },
                Math::fVector2 { 0, 5 }, Math::fVector2 { -3, -1 }, Math::fVector2 { 3, -1 });
            goto addColor;
        }

        if (selectedIndex != ~0 && ImGui::TreeNode("Edit Body")) {
            if (auto circle = Selected()->body->shape.As<Physics2D::CircleShape>()) {
                ImGui::EditScalar("Radius", circle->radius);
            }

            ImGui::EditScalar("Mass", Selected()->body->mass, 1, Math::fRange { 0, INFINITY });
            ImGui::EditColor ("Tint", Selected()->color);


            ImGui::TreePop();
        }

        if (ImGui::Button(onPause ? "Unpause" : "Pause")) {
            onPause = !onPause;
        }
        if (onPause) {
            onPause += ImGui::Button("Step");
        }

        return;

        addColor:
        AddBodyTint(Math::fColor::random(gdevice.GetRand(), 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestPhysicsPlayground2D::Select(u32 toSelect) {
        Unselect();
        selectedIndex = toSelect;
        if (selectedIndex != ~0) {
            selectedIsStatic = Selected()->body->IsStatic();
            Selected()->body->type = Physics2D::BodyType::STATIC;
            originalVelocity = Selected()->body->velocity;
            Selected()->body->velocity = 0;
        }
        SetControlPoints();
    }

    void TestPhysicsPlayground2D::Unselect() {
        if (selectedIndex != ~0 && !selectedIsStatic) {
            Selected()->body->type = Physics2D::BodyType::DYNAMIC;
            Selected()->body->velocity = originalVelocity;
        }
        selectedIndex = ~0;
        controlPointCount = 0;
        selectedControl = ~0;
    }

    Ref<TestPhysicsPlayground2D::Object> TestPhysicsPlayground2D::Selected() {
        return selectedIndex == ~0 ? nullptr : Refer(bodyData[selectedIndex]);
    }

    void TestPhysicsPlayground2D::AddNewPoint(const Math::fVector2& point, const Math::fColor& color) {
        auto meshp = worldMesh.NewBatch();
        meshp.PushV({ point + Math::fVector2 { -0.5f, -0.5f }, color });
        meshp.PushV({ point + Math::fVector2 { -0.5f, +0.5f }, color });
        meshp.PushV({ point + Math::fVector2 { +0.5f, -0.5f }, color });
        meshp.PushV({ point + Math::fVector2 { +0.5f, +0.5f }, color });
        meshp.PushI(0, 1, 2);
        meshp.PushI(1, 2, 3);
    }

    u32 TestPhysicsPlayground2D::FindAt(const Math::fVector2& mousePos) const {
        const Physics2D::CircleShape mouseCollider = { 0.0f };
        for (u32 i = 0; i < bodyData.size(); ++i) {
            const auto& b = bodyData[i];
            if (Physics2D::Collision::OverlapShapeDyn(mouseCollider, mousePos, b.body->shape, b.body->GetTransform())) {
                return i;
            }
        }
        return ~0;
    }

    void TestPhysicsPlayground2D::AddBodyTint(const Math::fColor& color) {
        bodyData.emplace_back(world.bodies.back(), color);
    }

    void TestPhysicsPlayground2D::SetControlPoints() {
        if (selectedIndex == ~0) return;
        const auto shape = Selected()->body->shape;
        switch (shape->TypeIndex()) {
            case Physics2D::CIRCLE: {
                controlPoints[0] = Math::fVector2::unit_x(shape.As<Physics2D::CircleShape>()->radius);
                controlPointCount = 1;
                break;
            }
            case Physics2D::EDGE: {
                const auto edge = shape.As<Physics2D::EdgeShape>();
                const Math::fVector2 prepend = (edge->start - edge->end).norm(edge->radius);
                controlPoints[0] = edge->start;
                controlPoints[1] = edge->end;
                controlPoints[2] = edge->start + Math::fVector2 { prepend.y, -prepend.x };
                controlPointCount = 3;
                break;
            }
            case Physics2D::TRIANGLE: {
                const auto tri = shape.As<Physics2D::TriangleShape>();
                controlPoints[0] = tri->a;
                controlPoints[1] = tri->b;
                controlPoints[2] = tri->c;
                controlPointCount = 3;
                break;
            }
            case Physics2D::MAX:
                break;
        }
    }

    void TestPhysicsPlayground2D::UpdateBodyFromControl() {
        auto shape = Selected()->body->shape;
        switch (shape->TypeIndex()) {
            case Physics2D::CIRCLE: {
                shape.As<Physics2D::CircleShape>()->radius = controlPoints[0].len();
                break;
            }
            case Physics2D::EDGE: {
                auto edge = shape.As<Physics2D::EdgeShape>();
                edge->start  = controlPoints[0];
                edge->end    = controlPoints[1];
                edge->radius = controlPoints[2].dist(controlPoints[0]);
                break;
            }
            case Physics2D::TRIANGLE: {
                auto tri = shape.As<Physics2D::TriangleShape>();
                tri->a = controlPoints[0];
                tri->b = controlPoints[1];
                tri->c = controlPoints[2];
                break;
            }
            case Physics2D::MAX:
                break;
        }
    }
}
