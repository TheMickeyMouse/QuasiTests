#include "TestPhysicsPlayground2D.h"

#include <imgui.h>

#include "VertexBlueprint.h"
#include "Extension/ImGuiExt.h"
#include "Meshes/Circle.h"
#include "Meshes/Stadium.h"

namespace Test {
    void TestPhysicsPlayground2D::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(2048, 2048);
        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));

        world = { { 0, -40.0f } };
        scene.SetProjection(Math::Matrix3D::ortho_projection({ -40, 40, -30, 30, -1, 1 }));

        world.CreateBody<Physics2D::RectShape>(
            { { 0, -30 }, Physics2D::BodyType::STATIC, 0.0f },
            100, 2); // floor
        AddBodyTint(Math::fColor::BETTER_GRAY());
        world.CreateBody<Physics2D::RectShape>(
            { { -102, 50 }, Physics2D::BodyType::STATIC, 0.0f },
            2, 82); // left side
        AddBodyTint(Math::fColor::BETTER_GRAY());
        world.CreateBody<Physics2D::RectShape>(
            { { +102, 50 }, Physics2D::BodyType::STATIC, 0.0f },
            2, 82); // right side
        AddBodyTint(Math::fColor::BETTER_GRAY());
        world.CreateBody<Physics2D::RectShape>(
            { { 0, 130 }, Physics2D::BodyType::STATIC, 0.0f },
            100, 2); // ceiling
        AddBodyTint(Math::fColor::BETTER_GRAY());
    }

    void TestPhysicsPlayground2D::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        const auto& imguiio = ImGui::GetIO();
        if (!imguiio.WantCaptureMouse && !imguiio.WantCaptureKeyboard) {
            const auto& mouse = gdevice.GetIO().Mouse;
            const Math::fVector2
                rawMouse = mouse.GetMousePos().map({ -1, 1, 1, -1 }, { -40, 40, -30, 30 }),
                mousePos = rawMouse * zoomFactor + cameraPosition;

            if (mouse.LeftOnPress()) {
                SelectControl(mousePos);
                if (controlIndex == ~0) {
                    if (const auto s = FindAt(mousePos); s != ~0) {
                        Select(s);
                        selectOffset = Selected()->body->position - mousePos;
                    } else Unselect();
                }
            }
            if (mouse.LeftPressed()) {
                if (controlIndex != ~0) {
                    EditControl(mousePos);
                } else if (selectedIndex != ~0) {
                    Selected()->body->position = mousePos + selectOffset;
                }
            }

            if (mouse.LeftOnRelease()) {
                controlIndex = ~0;
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

            if (mouse.RightPressed() && selectedIndex != ~0 && !selectedIsStatic) {
                hasAddedForce = true;
            }

            if (mouse.RightOnRelease() && selectedIndex != ~0 && !selectedIsStatic) {
                const bool scale = gdevice.GetIO().Keyboard.KeyPressed(IO::Key::LCONTROL);
                addedVelocity -= (scale ? 10.0f : 1.0f) * (mousePos - Selected()->body->position);
                Unselect();
                hasAddedForce = false;
            }

            zoomFactor *= (float)(1 - 0.05f * mouse.GetMouseScrollDelta().y);
        }

        worldUpdate:
        if (onPause & 1) return;
        world.Update(std::min(deltaTime, 1 / 60.0f), 32);
        onPause = onPause == 2 ? 1 : 0;
    }

    void TestPhysicsPlayground2D::OnRender(Graphics::GraphicsDevice& gdevice) {
        worldMesh.Clear();

        u32 i = 0;

        for (const auto& [body, color] : bodyData) {
            Qmatch$(body->shape, (
                instanceof (const Physics2D::CircleShape& circ) {
                    Graphics::MeshUtils::CircleCreator::Merge(
                        { 16 },
                        QGLCreateBlueprint$(Vertex, (
                            in (Position),
                            out (Position) = Position * circ.radius + body->position;,
                            out (Color)    = color;
                        )),
                        worldMesh
                    );
                },
                instanceof (const Physics2D::CapsuleShape& cap) {
                    Graphics::MeshUtils::StadiumCreator::Merge(
                        { .start = 0, .end = cap.forward, .radius = cap.radius, .subdivisions = 4 },
                        QGLCreateBlueprint$(Vertex, (
                            in (Position),
                            out (Position) = Position + body->position;,
                            out (Color)    = color;
                        )), worldMesh
                    );
                },
                instanceof (const Physics2D::TriangleShape& tri) {
                    worldMesh.NewBatch().PushTri(
                        { body->position + tri.points[0], color },
                        { body->position + tri.points[1], color },
                        { body->position + tri.points[2], color }
                    );
                },
                instanceof (const Physics2D::RectShape& rect) {
                    worldMesh.NewBatch().PushPolygon({
                        { body->position + rect.Corner(false, false), color },
                        { body->position + rect.Corner(true,  false), color },
                        { body->position + rect.Corner(true,  true ), color },
                        { body->position + rect.Corner(false, true ), color },
                    });
                },
                instanceof (const Physics2D::QuadShape& quad) {
                    worldMesh.NewBatch().PushPolygon({
                        { body->position + quad.points[0], color },
                        { body->position + quad.points[1], color },
                        { body->position + quad.points[2], color },
                        { body->position + quad.points[3], color },
                    });
                },
                instanceof (const Physics2D::DynPolyShape& poly) {
                    worldMesh.NewBatch().PushPolygon(
                        std::views::transform(poly.points,
                            [&] (const Math::fVector2& p) {
                                return Vertex { body->position + p, color };
                            })
                    );
                }
            ))
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

        DrawControlPoints();

        Math::fRect3D viewport = Math::fRect3D { -40, 40, -30, 30, -1, 1 } * zoomFactor + cameraPosition;
        viewport.min.z = -1;
        viewport.max.z = +1;
        scene.Draw(worldMesh, Graphics::UseArgs({{ "u_projection", Math::Matrix3D::ortho_projection(viewport) }}, false));
    }

    void TestPhysicsPlayground2D::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        auto& rand = gdevice.GetRand();
        if (ImGui::Button("+ New Circle")) {
            AddRandomCircle(rand);
        }
        if (ImGui::Button("+ New Capsule")) {
            AddRandomCapsule(rand);
        }
        if (ImGui::Button("+ New Triangle")) {
            AddRandomTri(rand);
        }
        if (ImGui::Button("+ New Rect")) {
            AddRandomRect(rand);
        }
        if (ImGui::Button("+ New Quad")) {
            AddRandomQuad(rand);
        }
        if (ImGui::Button("+ New Polygon")) {
            AddRandomPolygon(rand);
        }

        if (ImGui::Button("Add 10 Random Shapes")) {
            for (i32 i = 0; i < 10; ++i)
                (this->*(rand.Choose({
                    &AddRandomCircle, &AddRandomCapsule, &AddRandomTri,
                    &AddRandomRect,   &AddRandomQuad,    &AddRandomPolygon })))(rand);
        }

        if (selectedIndex != ~0 && ImGui::TreeNode("Edit Body")) {
            constexpr const char* SHAPE_NAMES[] = {
                "Circle", "Capsule", "Rect", "Triangle", "Quad", "Polygon"
            };
            ImGui::Text("Type: %s", SHAPE_NAMES[Selected()->body->shape.ID()]);

            EditBody();
            ImGui::EditScalar("Mass", Selected()->body->mass, 1, Math::fRange { 0, INFINITY });
            ImGui::EditColor ("Tint", Selected()->color);

            if (ImGui::Button("Delete")) {
                Selected()->body.Remove();
                bodyData.erase(bodyData.begin() + selectedIndex);
                selectedIndex = ~0;
                controlIndex = ~0;
            }

            ImGui::TreePop();
        }

        if (ImGui::Button(onPause ? "Unpause" : "Pause")) {
            onPause = !onPause;
        }
        if (onPause) {
            onPause += ImGui::Button("Step");
        }
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
            Selected()->body->velocity = 0;
            addedVelocity = 0;
        }
    }

    void TestPhysicsPlayground2D::Unselect() {
        if (selectedIndex != ~0 && !selectedIsStatic) {
            Selected()->body->type = Physics2D::BodyType::DYNAMIC;
            Selected()->body->velocity = addedVelocity;
        }
        selectedIndex = ~0;
        controlIndex = ~0;
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
        const Physics2D::TransformedShape mouseCollider = Physics2D::CircleShape { 0.0f }.Transform(mousePos);
        for (u32 i = 0; i < bodyData.size(); ++i) {
            const auto& b = bodyData[i];
            if (b.body->OverlapsWith(mouseCollider)) {
                return i;
            }
        }
        return ~0;
    }

    void TestPhysicsPlayground2D::AddBodyTint(const Math::fColor& color) {
        bodyData.emplace_back(world.bodies.back(), color);
    }

    void TestPhysicsPlayground2D::SelectControl(const Math::fVector2& mouse) {
        if (selectedIndex == ~0) return;

        Qmatch$ (Selected()->body->shape, (
            instanceof (const Physics2D::CircleShape& circ) {
                SelectControlPoint(mouse, { circ.radius, 0 }, 0);
            },
            instanceof (const Physics2D::CapsuleShape& cap) {
                const Math::fVector2 rf = cap.forward + cap.forward.perpend().norm(cap.radius);
                SelectControlPoint(mouse, cap.forward, 0);
                SelectControlPoint(mouse, rf, 1);
            },
            instanceof (const Physics2D::RectShape& rect) {
                SelectControlPoint(mouse, rect.Corner(false, false), 0);
                SelectControlPoint(mouse, rect.Corner(false, true),  1);
                SelectControlPoint(mouse, rect.Corner(true,  false), 2);
                SelectControlPoint(mouse, rect.Corner(true,  true),  3);
            },
            instanceof (const Physics2D::TriangleShape& tri) {
                SelectControlPoint(mouse, tri.points[0], 0);
                SelectControlPoint(mouse, tri.points[1], 1);
                SelectControlPoint(mouse, tri.points[2], 2);
            },
            instanceof (const Physics2D::QuadShape& quad) {
                SelectControlPoint(mouse, quad.points[0], 0);
                SelectControlPoint(mouse, quad.points[1], 1);
                SelectControlPoint(mouse, quad.points[2], 2);
                SelectControlPoint(mouse, quad.points[3], 3);
            },
            instanceof (const Physics2D::DynPolyShape& poly) {
                for (u32 i = 0; i < poly.Size(); ++i) {
                    SelectControlPoint(mouse, poly.points[i], i);
                }
            }
        ))
    }

    void TestPhysicsPlayground2D::SelectControlPoint(const Math::fVector2& mouse, const Math::fVector2& control, u32 i) {
        const Math::fVector2& origin = Selected()->body->position;
        if (OverlapShapes(Physics2D::CircleShape { 0.0f }.Transform(mouse),
                          Physics2D::CircleShape { 2.0f }.Transform(origin + control))) {
            controlIndex = i;
            controlOffset = control - mouse;
        }
    }

    void TestPhysicsPlayground2D::EditControl(const Math::fVector2& mouse) {
        if (selectedIndex == ~0) return;

        Qmatch$ (Selected()->body->shape, (
            instanceof (Physics2D::CircleShape& circ) ({
                Math::fVector2 r = { circ.radius, 0 };
                EditControlPoint(mouse, r, 0);
                circ.radius = r.len();
            });,
            instanceof (Physics2D::CapsuleShape& cap) {
                Math::fVector2 rf = cap.forward + cap.forward.perpend().norm(cap.radius);
                EditControlPoint(mouse, cap.forward, 0);
                EditControlPoint(mouse, rf, 1);
                cap.radius = (rf - cap.forward).len();
            },
            instanceof (Physics2D::RectShape& rect) ({
                Math::fVector2 corners[] = {
                    rect.Corner(false, false),
                    rect.Corner(false, true),
                    rect.Corner(true,  false),
                    rect.Corner(true,  true),
                };

                EditControlPoint(mouse, corners[0], 0);
                EditControlPoint(mouse, corners[1], 1);
                EditControlPoint(mouse, corners[2], 2);
                EditControlPoint(mouse, corners[3], 3);

                rect.hx = std::abs(corners[controlIndex].x);
                rect.hy = std::abs(corners[controlIndex].y);
            });,
            instanceof (Physics2D::TriangleShape& tri) {
                EditControlPoint(mouse, tri.points[0], 0);
                EditControlPoint(mouse, tri.points[1], 1);
                EditControlPoint(mouse, tri.points[2], 2);
            },
            instanceof (Physics2D::QuadShape& quad) {
                EditControlPoint(mouse, quad.points[0], 0);
                EditControlPoint(mouse, quad.points[1], 1);
                EditControlPoint(mouse, quad.points[2], 2);
                EditControlPoint(mouse, quad.points[3], 3);
            },
            instanceof (Physics2D::DynPolyShape& poly) {
                for (u32 i = 0; i < poly.Size(); ++i) {
                    EditControlPoint(mouse, poly.points[i], i);
                }
            }
        ))
    }

    void TestPhysicsPlayground2D::EditControlPoint(const Math::fVector2& mouse, Math::fVector2& control, u32 i) {
        if (controlIndex != i) return;
        control = mouse + controlOffset;
    }

    void TestPhysicsPlayground2D::DrawControlPoints() {
        if (selectedIndex == ~0) return;
        const Math::fColor controlColorGreen = Math::fColor::GREEN();

        const Math::fVector2& pos = Selected()->body->position;
        Qmatch$ (Selected()->body->shape, (
            instanceof (Physics2D::CircleShape& circ) ({
                AddNewPoint(pos + Math::fVector2::unit_x(circ.radius), controlColorGreen);
            });,
            instanceof (Physics2D::CapsuleShape& cap) {
                AddNewPoint(pos + cap.forward, controlColorGreen);
                AddNewPoint(pos + cap.forward + cap.forward.perpend().norm(cap.radius), controlColorGreen);
            },
            instanceof (Physics2D::RectShape& rect) ({
                AddNewPoint(pos + rect.Corner(false, false), controlColorGreen);
                AddNewPoint(pos + rect.Corner(false, true),  controlColorGreen);
                AddNewPoint(pos + rect.Corner(true,  false), controlColorGreen);
                AddNewPoint(pos + rect.Corner(true,  true),  controlColorGreen);
            });,
            instanceof (Physics2D::TriangleShape& tri) {
                AddNewPoint(pos + tri.points[0], controlColorGreen);
                AddNewPoint(pos + tri.points[1], controlColorGreen);
                AddNewPoint(pos + tri.points[2], controlColorGreen);
            },
            instanceof (Physics2D::QuadShape& quad) {
                AddNewPoint(pos + quad.points[0], controlColorGreen);
                AddNewPoint(pos + quad.points[1], controlColorGreen);
                AddNewPoint(pos + quad.points[2], controlColorGreen);
                AddNewPoint(pos + quad.points[3], controlColorGreen);
            },
            instanceof (Physics2D::DynPolyShape& poly) {
                for (u32 i = 0; i < poly.Size(); ++i) {
                    AddNewPoint(pos + poly.points[i], controlColorGreen);
                }
            }
        ))
    }

    void TestPhysicsPlayground2D::EditBody() {
        Qmatch$ (Selected()->body->shape, (
            instanceof (Physics2D::CircleShape& circ) {
                ImGui::EditScalar("Radius", circ.radius, 0.2, Math::fRange { 0, 100 });
            },
            instanceof (Physics2D::CapsuleShape& cap) ({
                float len = cap.forward.len(), angle = cap.forward.angle();
                ImGui::EditScalar("Length", len,        0.2,  Math::fRange { 0, 100 });
                ImGui::EditScalar("Angle",  angle,      0.01, Math::fRange { 0, 100 });
                ImGui::EditScalar("Radius", cap.radius, 0.2,  Math::fRange { 0, 100 });
                cap.forward = Math::fVector2::from_polar(len, angle);
            });,
            instanceof (Physics2D::RectShape& rect) ({
                Math::fVector2 half = { rect.hx, rect.hy };
                ImGui::EditVector("Size", half);
                rect.hx = half.x;
                rect.hy = half.y;
            });,
            instanceof (Physics2D::TriangleShape& tri) {
                ImGui::EditVector("Point #1", tri.points[0]);
                ImGui::EditVector("Point #2", tri.points[1]);
                ImGui::EditVector("Point #3", tri.points[2]);
            },
            instanceof (Physics2D::QuadShape& quad) {
                ImGui::EditVector("Point #1", quad.points[0]);
                ImGui::EditVector("Point #2", quad.points[1]);
                ImGui::EditVector("Point #3", quad.points[2]);
                ImGui::EditVector("Point #4", quad.points[3]);
            },
            instanceof (Physics2D::DynPolyShape& poly) {
                char title[] = "Point #01";
                for (u32 i = 0; i < poly.Size(); ++i) {
                    title[7] = ((i + 1) / 10) + '0';
                    title[8] = ((i + 1) % 10) + '0';
                    ImGui::EditVector(Str { title, Q_STRLIT_LEN(title) }, poly.points[i]);
                }
            }
        ))
    }

    void TestPhysicsPlayground2D::AddRandomCircle(Math::RandomGenerator& rand) {
        world.CreateBody<Physics2D::CircleShape>({ 0, Physics2D::BodyType::DYNAMIC, 1 },
            rand.GetExponential(2.0f, 15.0f));
        AddBodyTint(Math::fColor::random(rand, 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::AddRandomCapsule(Math::RandomGenerator& rand) {
        const float r = rand.GetExponential(2.0f, 15.0f);
        world.CreateBody<Physics2D::CapsuleShape>({ 0, Physics2D::BodyType::DYNAMIC, 1 },
            Math::fVector2::random_on_unit(rand) * r * rand.GetExponential(0.8f, 2.0f), r);
        AddBodyTint(Math::fColor::random(rand, 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::AddRandomTri(Math::RandomGenerator& rand) {
        const float angle1 = rand.Get(0, Math::TAU),
                    angle2 = std::fmod(angle1 + rand.Get(Math::PI * 0.2f, Math::PI * 0.8f), Math::TAU),
                    angle3 = std::fmod(angle2 + rand.Get(Math::PI * 0.2f, Math::PI * 0.8f), Math::TAU);

        const float r1 = rand.Get(6.0f, 15.0f),
                    r2 = r1 * rand.GetExponential(0.8f, 1.2f),
                    r3 = r1 * rand.GetExponential(0.8f, 1.2f);

        world.CreateBody<Physics2D::TriangleShape>({ 0, Physics2D::BodyType::DYNAMIC, 1 },
            Array {
                Math::fVector2::from_polar(r1, angle1),
                Math::fVector2::from_polar(r2, angle2),
                Math::fVector2::from_polar(r3, angle3)
            });
        AddBodyTint(Math::fColor::random(rand, 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::AddRandomRect(Math::RandomGenerator& rand) {
        const float width  = rand.Get(3.0f, 15.0f),
                    height = width * rand.Get(0.8f, 1.2f);

        world.CreateBody<Physics2D::RectShape>({ 0, Physics2D::BodyType::DYNAMIC, 1 },
            width, height);

        AddBodyTint(Math::fColor::random(rand, 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::AddRandomQuad(Math::RandomGenerator& rand) {
        const float angle1 = rand.Get(0, Math::TAU),
                    angle2 = std::fmod(angle1 + rand.Get(Math::PI * 0.3f, Math::PI * 0.5f), Math::TAU),
                    angle3 = std::fmod(angle2 + rand.Get(Math::PI * 0.3f, Math::PI * 0.5f), Math::TAU),
                    angle4 = std::fmod(angle3 + rand.Get(Math::PI * 0.3f, Math::PI * 0.5f), Math::TAU);

        const float r1 = rand.Get(6.0f, 15.0f),
                    r2 = r1 * rand.GetExponential(0.8f, 1.2f),
                    r3 = r1 * rand.GetExponential(0.8f, 1.2f),
                    r4 = r1 * rand.GetExponential(0.8f, 1.2f);

        world.CreateBody<Physics2D::QuadShape>({ 0, Physics2D::BodyType::DYNAMIC, 1 },
            Array {
                Math::fVector2::from_polar(r1, angle1),
                Math::fVector2::from_polar(r2, angle2),
                Math::fVector2::from_polar(r3, angle3),
                Math::fVector2::from_polar(r4, angle4),
            });

        AddBodyTint(Math::fColor::random(rand, 0.8, 0.8));
    }

    void TestPhysicsPlayground2D::AddRandomPolygon(Math::RandomGenerator& rand) {
        u32 pointCount = 3;
        float prob = 0.6;
        while (rand.GetBool(prob)) {
            ++pointCount;
            prob *= 0.8f;
        }

        Vec<Math::fVector2> points;
        points.resize(pointCount);
        points[0] = Math::fVector2::from_polar(rand.Get(6.0f, 15.0f), rand.Get(0, Math::TAU));
        const float baseAngle = Math::TAU / (float)(pointCount - 1);
        for (u32 i = 1; i < points.size(); ++i) {
            points[i] = Math::fComplex::expi(rand.Get(baseAngle * 0.2f, baseAngle * 0.8f)).rotate(points[i - 1]) * rand.Get(0.8f, 1.2f);
        }

        world.CreateBody<Physics2D::DynPolyShape>({ 0, Physics2D::BodyType::DYNAMIC, 1 }, std::move(points));

        AddBodyTint(Math::fColor::random(rand, 0.8, 0.8));
    }
}