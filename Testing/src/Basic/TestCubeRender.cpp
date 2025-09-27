#include "TestCubeRender.h"

#include "GLs/VertexBlueprint.h"
#include "GUI/ImGuiExt.h"
#include "Meshes/Cube.h"

namespace Test {
    void TestCubeRender::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Graphics::VertexColor3D>(4 * 6, 12);

        using namespace Math;

        u32 i = 0;
        cube = Graphics::Meshes::Cube().Create(QGLCreateBlueprint$(Graphics::VertexColor3D, (
            in (Position),
            out (Position) = Position;,
            out (Color) = fColor::Better::Colors[i / 4];,
            i++;
        )));

        render.UseShaderFromFile(RES("shader.vert"), RES("shader.frag"));
        render.SetProjection(projection);
    }

    void TestCubeRender::OnRender(Graphics::GraphicsDevice& gdevice) {
        render.Draw(cube, Graphics::UseArgs({{ "u_alpha", alpha }}));
    }

    void TestCubeRender::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditTransform("Transform", cube.modelTransform, 0.01f);
        ImGui::EditScalar("Transparency", alpha, 0.01f, fRange { 0, 1 });
    }

    void TestCubeRender::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
