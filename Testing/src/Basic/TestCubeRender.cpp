#include "TestCubeRender.h"
#include "Quad.h"

#include "imgui.h"

namespace Test {
    void TestCubeRender::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<VertexColor3D>(4 * 6, 12);

        using Graphics::Primitives::Quad;
        using namespace Maths;

        cube = Graphics::Mesh<VertexColor3D>::Combine<6>(std::array {
                Quad({ +1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 })
                    .IntoMesh<VertexColor3D>()
                    .ApplyMaterial(&VertexColor3D::Color, colorf::RED()),
                Quad({ -1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 })
                    .IntoMesh<VertexColor3D>()
                    .ApplyMaterial(&VertexColor3D::Color, colorf::CYAN()),
                Quad({ 0, +1, 0 }, { 1, 0, 0 }, { 0, 0, 1 })
                    .IntoMesh<VertexColor3D>()
                    .ApplyMaterial(&VertexColor3D::Color, colorf::GREEN()),
                Quad({ 0, -1, 0 }, { 1, 0, 0 }, { 0, 0, 1 })
                    .IntoMesh<VertexColor3D>()
                    .ApplyMaterial(&VertexColor3D::Color, colorf::MAGENTA()),
                Quad({ 0, 0, +1 }, { 1, 0, 0 }, { 0, 1, 0 })
                    .IntoMesh<VertexColor3D>()
                    .ApplyMaterial(&VertexColor3D::Color, colorf::BLUE()),
                Quad({ 0, 0, -1 }, { 1, 0, 0 }, { 0, 1, 0 })
                    .IntoMesh<VertexColor3D>()
                    .ApplyMaterial(&VertexColor3D::Color, colorf::YELLOW()),
                }
            );

        render.BindMeshes(cube);

        render.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        render.SetProjection(projection);
    }

    void TestCubeRender::OnRender(Graphics::GraphicsDevice& gdevice) {
        Maths::mat3D mat = Maths::mat3D::transform(modelTranslation, modelScale, modelRotation);

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
