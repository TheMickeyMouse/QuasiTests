#include "TestPhysicsPlayground2D.h"

#include <imgui.h>

#include "GLs/VertexBlueprint.h"
#include "GUI/ImGuiExt.h"
#include "Utils/Iter/MapIter.h"
#include "Meshes/Circle.h"
#include "Meshes/Stadium.h"

namespace Test {
    void TestPhysicsPlayground2D::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(2048, 2048);
        scene.UseShaderFromFile(res("shader.vert").IntoCStr(), res("shader.frag").IntoCStr());

        world = { { 0, -80.0f } };
        scene.SetProjection(Math::Matrix3D::OrthoProjection({ { -40, -30, -1 }, { 40, 30, 1 } }));

        using namespace Math;
        world.CreateBody<Physics2D::RectShape>(
            { .position = {    0, -30 }, .type = Physics2D::BodyType::STATIC, .density = 0.0f },
            100, 2); // floor
        AddBodyTint(fColor::Gray());
        world.CreateBody<Physics2D::RectShape>(
            { .position = { -102,  50 }, .type = Physics2D::BodyType::STATIC, .density = 0.0f },
            2, 82); // left side
        AddBodyTint(fColor::Gray());
        world.CreateBody<Physics2D::RectShape>(
            { .position = { +102,  50 }, .type = Physics2D::BodyType::STATIC, .density = 0.0f },
            2, 82); // right side
        AddBodyTint(fColor::Gray());
        world.CreateBody<Physics2D::RectShape>(
            { .position = {    0, 130 }, .type = Physics2D::BodyType::STATIC, .density = 0.0f },
            100, 2); // ceiling
        AddBodyTint(fColor::Gray());

        camera.speed = 15.0f;
        camera.scale = 15.0f;
    }

    void TestPhysicsPlayground2D::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);

        const auto& imguiio = ImGui::GetIO();
        if (!imguiio.WantCaptureMouse && !imguiio.WantCaptureKeyboard) {
            const Math::fRect2D viewport = camera.GetViewport();
            const auto& mouse = gdevice.GetIO().Mouse;

            const Math::fv2 mousePos = (((Math::fv2)mouse.FlipMouseY(mouse.GetMousePos()) + 1) * 0.5f).MapFromUnit(viewport);

            if (mouse.LeftOnPress()) {
                // Debug::QInfo$("mouse at ({})", mousePos);
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

            if (mouse.RightOnPress()) {
                const u32 target = FindAt(mousePos);
                if (target != ~0 && bodyData[target].body->IsDynamic()) {
                    Select(target);
                    forceAddedPosition = mousePos;
                }
            }

            if (mouse.RightPressed() && selectedIndex != ~0 && !selectedIsStatic) {
                hasAddedForce = true;
            }

            if (mouse.RightOnRelease() && selectedIndex != ~0 && !selectedIsStatic) {
                const bool scale = gdevice.GetIO().Keyboard.KeyPressed(IO::Key::LCONTROL);
                addedVelocity -= (scale ? 10.0f : 1.0f) * (mousePos - forceAddedPosition);
                Unselect();
                hasAddedForce = false;
            }
        }

        worldUpdate:
        if (onPause & 1) return;
        world.Update(std::min(deltaTime, 1 / 60.0f), 32);
        onPause = onPause == 2 ? 1 : 0;
    }

    void TestPhysicsPlayground2D::OnRender(Graphics::GraphicsDevice& gdevice) {
        worldMesh.Clear();

        u32 i = 0;

        using namespace Math;
        for (const auto& [body, color] : bodyData) {
            const auto& t = body->GetTransform();
            Qmatch$(body->shape, (
                instanceof (const Physics2D::CircleShape& circ) {
                    Graphics::Meshes::Circle(3).Merge(
                        QGLCreateBlueprint$(Vertex, (
                            in (Position),
                            out (Position) = Position * circ.radius + t.position;,
                            out (Color)    = color;
                        )),
                        worldMesh.NewBatch()
                    );
                    AddNewPoint(t.Transform({ circ.radius, 0 }), fColor::White());
                },
                instanceof (const Physics2D::CapsuleShape& cap) {
                    Graphics::Meshes::Stadium(-cap.forward, cap.forward, cap.radius, 4).Merge(
                        QGLCreateBlueprint$(Vertex, (
                            in (Position),
                            out (Position) = t * Position;,
                            out (Color)    = color;
                        )), worldMesh.NewBatch()
                    );
                    AddNewPoint(t.Transform(cap.forward.Perpend() * (cap.invLength * cap.radius)), fColor::White());
                },
                instanceof (const Physics2D::TriangleShape& tri) {
                    worldMesh.NewBatch().PushTri(
                        { t * tri.points[0], color },
                        { t * tri.points[1], color },
                        { t * tri.points[2], color }
                    );
                },
                instanceof (const Physics2D::RectShape& rect) {
                    worldMesh.NewBatch().PushPolygon({
                        { t * rect.Corner(false, false), color },
                        { t * rect.Corner(true,  false), color },
                        { t * rect.Corner(true,  true ), color },
                        { t * rect.Corner(false, true ), color },
                    });
                },
                instanceof (const Physics2D::QuadShape& quad) {
                    worldMesh.NewBatch().PushPolygon({
                        { t * quad.points[0], color },
                        { t * quad.points[1], color },
                        { t * quad.points[2], color },
                        { t * quad.points[3], color },
                    });
                },
                instanceof (const Physics2D::DynPolygonShape& poly) {
                    worldMesh.NewBatch().PushPolygon(
                        poly.data.Iter()
                                 .Map(Operators::Member<&Physics2D::DynPolygonShape::PointWithInvDist::coords> {})
                                 .Map([&] (const Math::fv2& p) {
                                        return Vertex { body->rotation.Rotate(p) + body->position, color };
                        })
                    );
                }
            ))
            ++i;
        }

        const fRect2D viewport = camera.GetViewport();
        if (hasAddedForce) {
            const auto& mouse = gdevice.GetIO().Mouse;
            const fColor blue = fColor::Blue();
            const fv2 mousePos = (((fv2)mouse.FlipMouseY(mouse.GetMousePos()) + 1) * 0.5f).MapFromUnit(viewport),
                      direction = (forceAddedPosition - mousePos).Norm(0.2f);
            auto meshp = worldMesh.NewBatch();
            meshp.PushV({ forceAddedPosition + direction.Perpend(), blue });
            meshp.PushV({ forceAddedPosition - direction.Perpend(), blue });
            meshp.PushV({ mousePos, blue });
            meshp.PushI(0, 1, 2);
        }

        if (selectedIndex != ~0) {
            AddNewPoint(Selected()->body->position, fColor::Red());
        }

        DrawControlPoints();

        scene.Draw(worldMesh, Graphics::UseArgs({{ "u_projection", Matrix3D::OrthoProjection(viewport.AddZ({ -1, 1 })) }}, false));
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
            ImGui::Text("Type: %s", SHAPE_NAMES[Selected()->body->shape.GetTag()]);

            EditBody();
            ImGui::EditRotation2D("Rotation", Selected()->body->rotation);
            float m = Selected()->body->mass;
            ImGui::EditScalar("Mass", m, 1, fRange { 0, f32s::INFINITY });
            Selected()->body->SetMass(m);
            ImGui::EditColor ("Tint", Selected()->color);

            if (ImGui::Button("Delete")) {
                world.DeleteBody(Selected()->body);
                bodyData.Pop(selectedIndex);
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

        ImGui::Text("Total Body Count: %d", bodyData.Length());
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
            Selected()->body->angularVelocity = 0;
            addedVelocity = 0;
        }
    }

    void TestPhysicsPlayground2D::Unselect() {
        if (selectedIndex != ~0 && !selectedIsStatic) {
            Selected()->body->type = Physics2D::BodyType::DYNAMIC;
            Selected()->body->AddVelocityAt(forceAddedPosition, addedVelocity * Selected()->body->mass);
        }
        selectedIndex = ~0;
        controlIndex = ~0;
    }

    OptRef<TestPhysicsPlayground2D::Object> TestPhysicsPlayground2D::Selected() {
        return selectedIndex == ~0 ? nullptr : OptRefs::SomeRef(bodyData[selectedIndex]);
    }

    void TestPhysicsPlayground2D::AddNewPoint(const Math::fv2& point, const Math::fColor& color) {
        auto meshp = worldMesh.NewBatch();
        meshp.PushV({ point + Math::fv2 { -0.5f, -0.5f }, color });
        meshp.PushV({ point + Math::fv2 { -0.5f, +0.5f }, color });
        meshp.PushV({ point + Math::fv2 { +0.5f, -0.5f }, color });
        meshp.PushV({ point + Math::fv2 { +0.5f, +0.5f }, color });
        meshp.PushI(0, 1, 2);
        meshp.PushI(1, 2, 3);
    }

    u32 TestPhysicsPlayground2D::FindAt(const Math::fv2& mousePos) const {
        const Physics2D::Shape mouseCollider = Physics2D::CircleShape { 0.0f };
        for (u32 i = 0; i < bodyData.Length(); ++i) {
            const auto& b = bodyData[i];
            if (b.body->OverlapsWith(mouseCollider, mousePos)) {
                return i;
            }
        }
        return ~0;
    }

    void TestPhysicsPlayground2D::AddBodyTint(const Math::fColor& color) {
        bodyData.Push({ *world.bodies.Last(), color });
    }

    void TestPhysicsPlayground2D::SelectControl(const Math::fv2& mouse) {
        if (selectedIndex == ~0) return;

        const auto& t = Selected()->body->GetTransform();
        Qmatch$ (Selected()->body->shape, (
            instanceof (const Physics2D::CircleShape& circ) {
                SelectControlPoint(mouse, t * Math::fv2 { circ.radius, 0 }, 0);
            },
            instanceof (const Physics2D::CapsuleShape& cap) {
                const Math::fv2 rf = cap.forward + cap.forward.Perpend() * (cap.radius * cap.invLength);
                SelectControlPoint(mouse, t * cap.forward, 0);
                SelectControlPoint(mouse, t * rf, 1);
            },
            instanceof (const Physics2D::RectShape& rect) {
                SelectControlPoint(mouse, t * rect.Corner(false, false), 0);
                SelectControlPoint(mouse, t * rect.Corner(false, true),  1);
                SelectControlPoint(mouse, t * rect.Corner(true,  false), 2);
                SelectControlPoint(mouse, t * rect.Corner(true,  true),  3);
            },
            instanceof (const Physics2D::TriangleShape& tri) {
                SelectControlPoint(mouse, t * tri.points[0], 0);
                SelectControlPoint(mouse, t * tri.points[1], 1);
                SelectControlPoint(mouse, t * tri.points[2], 2);
            },
            instanceof (const Physics2D::QuadShape& quad) {
                SelectControlPoint(mouse, t * quad.points[0], 0);
                SelectControlPoint(mouse, t * quad.points[1], 1);
                SelectControlPoint(mouse, t * quad.points[2], 2);
                SelectControlPoint(mouse, t * quad.points[3], 3);
            },
            instanceof (const Physics2D::DynPolygonShape& poly) {
                for (u32 i = 0; i < poly.Size(); ++i) {
                    SelectControlPoint(mouse, t * poly.PointAt(i), i);
                }
            }
        ))
    }

    void TestPhysicsPlayground2D::SelectControlPoint(const Math::fv2& mouse, const Math::fv2& control, u32 i) {
        if (OverlapShapes(Physics2D::CircleShape { 0.0f }, mouse,
                          Physics2D::CircleShape { 2.0f }, control)) {
            controlIndex = i;
            controlOffset = control - mouse;
        }
    }

    void TestPhysicsPlayground2D::EditControl(const Math::fv2& mouse) {
        if (selectedIndex == ~0) return;

        Qmatch$ (Selected()->body->shape, (
            instanceof (Physics2D::CircleShape& circ) ({
                Math::fv2 r = { circ.radius, 0 };
                EditControlPoint(mouse, r, 0);
                circ.radius = r.Len();
            });,
            instanceof (Physics2D::CapsuleShape& cap) ({
                Math::fv2 f = cap.forward, rf = cap.forward + cap.forward.Perpend().Norm(cap.radius);
                EditControlPoint(mouse, f, 0);
                EditControlPoint(mouse, rf, 1);
                cap.SetForward(f);
                cap.radius = rf.Dist(cap.forward);
            });,
            instanceof (Physics2D::RectShape& rect) ({
                Math::fv2 corners[] = {
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
                tri.FixPolygon();
            },
            instanceof (Physics2D::QuadShape& quad) {
                EditControlPoint(mouse, quad.points[0], 0);
                EditControlPoint(mouse, quad.points[1], 1);
                EditControlPoint(mouse, quad.points[2], 2);
                EditControlPoint(mouse, quad.points[3], 3);
                quad.FixPolygon();
            },
            instanceof (Physics2D::DynPolygonShape& poly) {
                for (u32 i = 0; i < poly.Size(); ++i) {
                    EditControlPoint(mouse, poly.PointAt(i), i);
                }
                poly.FixPolygon();
            },
            else return;
        ))
        Selected()->body->SetShapeHasChanged();
    }

    void TestPhysicsPlayground2D::EditControlPoint(const Math::fv2& mouse, Math::fv2& control, u32 i) {
        if (controlIndex != i) return;
        const Math::fv2& origin = Selected()->body->position;
        const Math::Rotor2D& rotation = Selected()->body->rotation;
        control = rotation.InvRotate(mouse + controlOffset - origin);
    }

    void TestPhysicsPlayground2D::DrawControlPoints() {
        if (selectedIndex == ~0) return;
        const Math::fColor CONTROL_GREEN = Math::fColor::Green();

        const auto& t = Selected()->body->GetTransform();
        Qmatch$ (Selected()->body->shape, (
            instanceof (Physics2D::CircleShape& circ) ({
                AddNewPoint(t * Math::fv2(circ.radius, 0), CONTROL_GREEN);
            });,
            instanceof (Physics2D::CapsuleShape& cap) {
                AddNewPoint(t * cap.forward, CONTROL_GREEN);
                AddNewPoint(t * (cap.forward + cap.forward.Perpend() * (cap.invLength * cap.radius)), CONTROL_GREEN);
            },
            instanceof (Physics2D::RectShape& rect) ({
                AddNewPoint(t * rect.Corner(false, false), CONTROL_GREEN);
                AddNewPoint(t * rect.Corner(false, true),  CONTROL_GREEN);
                AddNewPoint(t * rect.Corner(true,  false), CONTROL_GREEN);
                AddNewPoint(t * rect.Corner(true,  true),  CONTROL_GREEN);
            });,
            instanceof (Physics2D::TriangleShape& tri) {
                AddNewPoint(t * tri.points[0], CONTROL_GREEN);
                AddNewPoint(t * tri.points[1], CONTROL_GREEN);
                AddNewPoint(t * tri.points[2], CONTROL_GREEN);
            },
            instanceof (Physics2D::QuadShape& quad) {
                AddNewPoint(t * quad.points[0], CONTROL_GREEN);
                AddNewPoint(t * quad.points[1], CONTROL_GREEN);
                AddNewPoint(t * quad.points[2], CONTROL_GREEN);
                AddNewPoint(t * quad.points[3], CONTROL_GREEN);
            },
            instanceof (Physics2D::DynPolygonShape& poly) {
                for (u32 i = 0; i < poly.Size(); ++i) {
                    AddNewPoint(t * poly.PointAt(i), CONTROL_GREEN);
                }
            }
        ))
    }

    void TestPhysicsPlayground2D::EditBody() {
        Qmatch$ (Selected()->body->shape, (
            instanceof (Physics2D::CircleShape& circ) {
                ImGui::EditScalar("Radius", circ.radius, 0.2, fRange { 0, 100 });
            },
            instanceof (Physics2D::CapsuleShape& cap) ({
                float len = cap.forward.Len();
                Math::Radians angle = cap.forward.PolarAngle();
                ImGui::EditScalar("Length", len,        0.2,  fRange { 0, 100 });
                ImGui::EditScalar("Angle",  *angle,     0.01, fRange { 0, Math::TAU });
                ImGui::EditScalar("Radius", cap.radius, 0.2,  fRange { 0, 100 });
                cap.SetForward(Math::fv2::FromPolar(len, angle));
            });,
            instanceof (Physics2D::RectShape& rect) ({
                Math::fv2 half = { rect.hx, rect.hy };
                ImGui::EditVector("Size", half);
                rect.hx = half.x;
                rect.hy = half.y;
            });,
            instanceof (Physics2D::TriangleShape& tri) {
                ImGui::EditVector("Point #1", tri.points[0]);
                ImGui::EditVector("Point #2", tri.points[1]);
                ImGui::EditVector("Point #3", tri.points[2]);
                tri.FixPolygon();
            },
            instanceof (Physics2D::QuadShape& quad) {
                ImGui::EditVector("Point #1", quad.points[0]);
                ImGui::EditVector("Point #2", quad.points[1]);
                ImGui::EditVector("Point #3", quad.points[2]);
                ImGui::EditVector("Point #4", quad.points[3]);
                quad.FixPolygon();
            },
            instanceof (Physics2D::DynPolygonShape& poly) {
                char title[] = "Point #01";
                for (u32 i = 0; i < poly.Size(); ++i) {
                    title[7] = ((i + 1) / 10) + '0';
                    title[8] = ((i + 1) % 10) + '0';
                    ImGui::EditVector(Str::Slice(title, sizeof(title) - 1), poly.PointAt(i));
                }
                poly.FixPolygon();
            }
        ))
    }

    void TestPhysicsPlayground2D::AddRandomCircle(Math::RandomGenerator& rand) {
        world.CreateBody<Physics2D::CircleShape>({ .position = 0, .density = 1 },
            rand.GetExponential(2.0f, 15.0f));
        AddBodyTint(Math::fColor::Random(rand));
    }

    void TestPhysicsPlayground2D::AddRandomCapsule(Math::RandomGenerator& rand) {
        const float r = rand.GetExponential(2.0f, 15.0f);
        world.CreateBody<Physics2D::CapsuleShape>({ .position = 0, .density = 1 },
            Math::fv2::RandomOnUnit(rand) * r * rand.GetExponential(0.8f, 2.0f), r);
        AddBodyTint(Math::fColor::Random(rand));
    }

    void TestPhysicsPlayground2D::AddRandomTri(Math::RandomGenerator& rand) {
        const float angle1 = rand.Get<float>(0, Math::TAU),
                    angle2 = std::fmod(angle1 + rand.Get(Math::PI * 0.2f, Math::PI * 0.8f), Math::TAU),
                    angle3 = std::fmod(angle2 + rand.Get(Math::PI * 0.2f, Math::PI * 0.8f), Math::TAU);

        const float r1 = rand.Get(6.0f, 15.0f),
                    r2 = r1 * rand.GetExponential(0.8f, 1.2f),
                    r3 = r1 * rand.GetExponential(0.8f, 1.2f);

        world.CreateBody({ .position = 0, .density = 1 },
            Physics2D::MakePolygon(Array {
                Math::fv2::FromPolar(r1, Math::Radians(angle1)),
                Math::fv2::FromPolar(r2, Math::Radians(angle2)),
                Math::fv2::FromPolar(r3, Math::Radians(angle3))
            }));
        AddBodyTint(Math::fColor::Random(rand));
    }

    void TestPhysicsPlayground2D::AddRandomRect(Math::RandomGenerator& rand) {
        const float width  = rand.Get(3.0f, 15.0f),
                    height = width * rand.Get(0.8f, 1.2f);

        world.CreateBody<Physics2D::RectShape>({ .position = 0, .density = 1 },
            width, height);

        AddBodyTint(Math::fColor::Random(rand));
    }

    void TestPhysicsPlayground2D::AddRandomQuad(Math::RandomGenerator& rand) {
        const float angle1 = rand.Get<float>(0, Math::TAU),
                    angle2 = std::fmod(angle1 + rand.Get(Math::PI * 0.3f, Math::PI * 0.5f), Math::TAU),
                    angle3 = std::fmod(angle2 + rand.Get(Math::PI * 0.3f, Math::PI * 0.5f), Math::TAU),
                    angle4 = std::fmod(angle3 + rand.Get(Math::PI * 0.3f, Math::PI * 0.5f), Math::TAU);

        const float r1 = rand.Get(6.0f, 15.0f),
                    r2 = r1 * rand.GetExponential(0.8f, 1.2f),
                    r3 = r1 * rand.GetExponential(0.8f, 1.2f),
                    r4 = r1 * rand.GetExponential(0.8f, 1.2f);

        world.CreateBody({ .position = 0, .density = 1 },
            Physics2D::MakePolygon(Array {
                Math::fv2::FromPolar(r1, Math::Radians(angle1)),
                Math::fv2::FromPolar(r2, Math::Radians(angle2)),
                Math::fv2::FromPolar(r3, Math::Radians(angle3)),
                Math::fv2::FromPolar(r4, Math::Radians(angle4)),
        }));

        AddBodyTint(Math::fColor::Random(rand));
    }

    void TestPhysicsPlayground2D::AddRandomPolygon(Math::RandomGenerator& rand) {
        u32 pointCount = 3;
        float prob = 0.6;
        while (rand.GetBool(prob)) {
            ++pointCount;
            prob *= 0.8f;
        }

        Vec points = Vec<Math::fv2>::WithSize(pointCount);
        points[0] = Math::fv2::FromPolar(rand.Get(6.0f, 15.0f), Math::Radians(rand.Get<float>(0, Math::TAU)));
        const float baseAngle = Math::TAU / (float)(pointCount - 1);
        for (u32 i = 1; i < points.Length(); ++i) {
            points[i] = Math::Rotor2D(Math::Radians(rand.Get(baseAngle * 0.2f, baseAngle * 0.8f))).Rotate(points[i - 1]) * rand.Get(0.8f, 1.2f);
        }

        world.CreateBody({ .position = 0, .density = 1 }, Physics2D::MakePolygon(points));

        AddBodyTint(Math::fColor::Random(rand));
    }
}
