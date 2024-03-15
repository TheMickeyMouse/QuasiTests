#include "TestCubeRender.h"
#include "Quad.h"

#include "imgui.h"
#include "MeshUtils.h"

namespace Test {
    void TestCubeRender::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Graphics::VertexColor3D>(4 * 6, 12);

        using Graphics::Primitives::Quad;
        using namespace Maths;

        cube = Graphics::MeshUtils::CubeMeshNorm(
            [](const Graphics::VertexNormal3D& vn3, uint i) {
                return Graphics::VertexColor3D { vn3.Position, colorf::color_id((int)(i / 4) + 1) };
            });

        render.BindMeshes(cube);

        render.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        render.SetProjection(projection);
    }

    void TestCubeRender::OnRender(Graphics::GraphicsDevice& gdevice) {
        const Maths::mat3D mat = Maths::mat3D::transform(modelTranslation, modelScale, modelRotation);

        render.SetCamera(mat);
        render.GetShader().Bind();
        render.GetShader().SetUniform1F("u_alpha", alpha);

        render.ResetData();
        render.Render();
    }

    void TestCubeRender::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Translation" , modelTranslation.begin(), 0.01f);
        ImGui::DragFloat3("Scale"       , modelScale.begin(),       0.01f);
        ImGui::DragFloat3("Rotation"    , modelRotation.begin(),    0.01f);
        ImGui::DragFloat ("Transparency", &alpha,                   0.01f);
    }

    void TestCubeRender::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
