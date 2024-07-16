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
        auto& mouse = gdevice.GetIO().Mouse;
        const Math::fVector2 mousePos = mouse.GetMousePos().map({ -1, 1, 1, -1 }, { -40, 40, -30, 30 });

        if (mouse.LeftOnPress()) {
            selectedControl = ~0;
            for (u32 i = 0; i < controlPointCount; ++i) {
                if (Physics2D::Collision::Between(
                    Physics2D::CircleShape { 0.0f }, mousePos,
                    Physics2D::CircleShape { 2.0f }, selected->body->position + controlPoints[i])) {
                    selectedControl = i;
                }
            }
            if (selectedControl == ~0) {
                if (const auto s = FindAt(mousePos)) { Select(s); }
                else Unselect();
            }
        }
        if (mouse.LeftPressed()) {
            if (selectedControl != ~0) {
                controlPoints[selectedControl] = mousePos - selected->body->position;
                UpdateBodyFromControl();
            } else if (selected) {
                selected->body->position = mousePos;
            }
        }

        if (mouse.MiddleOnPress()) lastDragPosition = mousePos;
        if (mouse.MiddlePressed()) {
            for (auto b : world.bodies) b->position -= lastDragPosition - mousePos;
            lastDragPosition = mousePos;
        }

        if (mouse.RightOnPress()) {
            Ref<Object> target = FindAt(mousePos);
            target = target && target->body->IsDynamic() ? target : nullptr;
            Select(target);
        }

        if (mouse.RightPressed() && selected) {
            hasAddedForce = true;
        }

        if (mouse.RightOnRelease() && selected && selected->body->IsDynamic()) {
            const bool scale = gdevice.GetIO().Keyboard.KeyPressed(IO::Key::LCONTROL);
            selected->body->velocity -= (scale ? 10.0f : 1.0f) * (mousePos - selected->body->position);
            Unselect();
            hasAddedForce = false;
        }

        world.Update(deltaTime, 8, 2);
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
            const Math::fVector2 mouse = gdevice.GetIO().Mouse.GetMousePos().map({ -1, 1, 1, -1 }, { -40, 40, -30, 30 }),
                                 direction = (selected->body->position - mouse).norm(0.5f);
            auto meshp = worldMesh.NewBatch();
            meshp.PushV({ selected->body->position + Math::fVector2 { direction.y, -direction.x }, blue });
            meshp.PushV({ selected->body->position + Math::fVector2 { -direction.y, direction.x }, blue });
            meshp.PushV({ mouse, blue });
            meshp.PushI(0, 1, 2);
        }

        if (selected) {
            AddNewPoint(selected->body->position, Math::fColor::RED());
        }

        if (controlPointCount) {
            for (u32 j = 0; j < controlPointCount; ++j)
                AddNewPoint(controlPoints[j] + selected->body->position, Math::fColor::GREEN());
        }

        scene.Draw(worldMesh, Graphics::UseArgs({{ "u_projection", scene->projection }}, false));
    }

    void TestPhysicsPlayground2D::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (ImGui::Button("+ New Circle")) {
            world.CreateBody<Physics2D::CircleShape>({ 0, Physics2D::BodyType::DYNAMIC, 1.0f }, 5.0f);
            goto addColor;
        } if (ImGui::Button("+ New Edge")) {
            world.CreateBody<Physics2D::EdgeShape>({ 0, Physics2D::BodyType::DYNAMIC, 1.0f }, Math::fVector2 { 0, 5 }, Math::fVector2 { 0, 0 }, 5.0f);
            goto addColor;
        }

        if (ImGui::TreeNode("Edit Body")) {
            if (auto circle = selected->body->shape.As<Physics2D::CircleShape>()) {
                ImGui::EditScalar("Radius", circle->radius);
            }

            ImGui::EditScalar("Mass", selected->body->mass);
            ImGui::EditColor ("Tint", selected->color);

            ImGui::TreePop();
        }

        return;

        addColor:
        AddBodyTint(Math::fColor::random(gdevice.GetRand(), 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestPhysicsPlayground2D::Select(Ref<Object> toSelect) {
        Unselect();
        selected = toSelect;
        if (selected && selected->body->IsDynamic()) selected->body->Disable();
        SetControlPoints();
    }

    void TestPhysicsPlayground2D::Unselect() {
        if (selected) selected->body->Enable();
        selected = nullptr;
        controlPointCount = 0;
        selectedControl = ~0;
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

    Ref<TestPhysicsPlayground2D::Object> TestPhysicsPlayground2D::FindAt(const Math::fVector2& mousePos) {
        const Physics2D::CircleShape mouseCollider = { 0.0f };
        for (auto& b : bodyData) {
            if (Physics2D::Collision::Between(mouseCollider, mousePos, b.body->shape, b.body->GetTransform())) {
                return b;
            }
        }
        return nullptr;
    }

    void TestPhysicsPlayground2D::AddBodyTint(const Math::fColor& color) {
        bodyData.emplace_back(world.bodies.back(), color);
    }

    void TestPhysicsPlayground2D::SetControlPoints() {
        if (!selected) return;
        const auto shape = selected->body->shape;
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
        auto shape = selected->body->shape;
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
