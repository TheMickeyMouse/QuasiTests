#include "TestBezierCurves.h"

#include "GUI/ImGuiExt.h"
#include "Utils/Math/Geometry.h"

namespace Test {
    void TestBezierCurves::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<BVertex>();

        scene.UseShaderFromFile(RES("bez.vert"), RES("bez.frag"));
        scene.SetProjection(Math::Matrix3D::OrthoProjection({ { -4, -3, -1 }, { 4, 3, 1 } }));

        mesh.PushTriangle(
            { { -1, -1 }, { 0,    0 }, Math::fColor::White() },
            { {  1, -1 }, { 0.5f, 0 }, Math::fColor::White() },
            { {  1,  1 }, { 1,    1 }, Math::fColor::White() }
        );
    }

    void TestBezierCurves::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        const auto& mouse = gdevice.GetIO().Mouse;
        const Math::fv2 mousePos = mouse.GetMousePos().As<float>().MapFromUnit({ 0, { 4, -3 } });

        switch (bezState) {
            case 0: if (mouse.LeftOnPress()) {
                // start bezier
                bezierPoints[0] = mousePos;
                ++bezState;
                break;
            }
            case 1: if (mouse.LeftOnRelease()) {
                bezierPoints[1] = mousePos;
                ++bezState;
                break;
            }
            case 2: if (mouse.LeftOnPress()) {
                bezierPoints[2] = mousePos;
                ++bezState;
                break;
            }
            case 3: if (mouse.LeftOnRelease()) {
                bezierPoints[3] = mousePos;
                bezState = 0;

                const Math::fLine2D a = Math::Lines::FromPoints(bezierPoints[1], bezierPoints[0]),
                                    b = Math::Lines::FromPoints(bezierPoints[3], bezierPoints[2]);
                const Math::fv2 middleControl = a.IntersectionUnclamped(b);
                const Math::fColor color = Math::fColor::Random(gdevice.GetRand());
                mesh.PushTriangle(
                    { bezierPoints[0], { 0,    0 }, color },
                    { middleControl,   { 0.5f, 0 }, color },
                    { bezierPoints[3], { 1,    1 }, color }
                );
                break;
            }
            default: break;
        }
    }

    void TestBezierCurves::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.Draw(mesh);
    }

    void TestBezierCurves::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditVector("Point 0", mesh.vertices[0].Position, 0.05f);
        ImGui::EditVector("Point 1", mesh.vertices[1].Position, 0.05f);
        ImGui::EditVector("Point 2", mesh.vertices[2].Position, 0.05f);
    }

    void TestBezierCurves::OnDestroy(Graphics::GraphicsDevice& gdevice) {

    }
}
