#include "TestBatchedTextured.h"
#include "TriIndices.h"

#include "imgui.h"

namespace Test {
    void TestBatchedTextured::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Vertex>(8, 4);

        using namespace Graphics;
        textures[0] = Texture::LoadPNG(res("image.png"));
        textures[1] = Texture::LoadPNG(res("img1.png"), { .params = { TextureParamPair::NearestSample() } });

        textures[0].Activate();
        textures[1].Activate();

        Vertex vertices[] = {
            { { -240.0f, -80.0f, 0 }, 1, { 0.0f, 0.0f }, 0 },
            { { -80.00f, -80.0f, 0 }, 1, { 1.0f, 0.0f }, 0 },
            { { -80.00f, +80.0f, 0 }, 1, { 1.0f, 1.0f }, 0 },
            { { -240.0f, +80.0f, 0 }, 1, { 0.0f, 1.0f }, 0 },

            { { +80.00f, -80.0f, 0 }, 1, { 0.0f, 0.0f }, 1 },
            { { +240.0f, -80.0f, 0 }, 1, { 1.0f, 0.0f }, 1 },
            { { +240.0f, +80.0f, 0 }, 1, { 1.0f, 1.0f }, 1 },
            { { +80.00f, +80.0f, 0 }, 1, { 0.0f, 1.0f }, 1 },
        };

        TriIndices indices[] = {
            { 0, 1, 2 }, { 2, 3, 0 },
            { 4, 5, 6 }, { 6, 7, 4 }
        };

        mesh = Mesh(
            std::vector(vertices, vertices + 8),
            std::vector(indices, indices + 4)
        );

        render.BindMeshes(mesh);
        render.SetProjection(projection);

        render.UseShaderFromFile(res("shader.vert"), res("shader.frag"));

        const int slots[] = { textures[0].Slot(), textures[1].Slot(), };
        render.GetShader().Bind();
        render.GetShader().SetUniform1IVec("u_textures", slots, 2);
        render.GetShader().Unbind();
    }

    void TestBatchedTextured::OnRender(Graphics::GraphicsDevice& gdevice) {
        Maths::mat3D mat = Maths::mat3D::transform(modelTranslation,
                                                   modelScale,
                                                   modelRotation);
        mesh.ApplyMaterial(&Vertex::Color, color);

        render.SetCamera(mat);
        render.ResetData();
        render.Render();
    }

    void TestBatchedTextured::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Translation", modelTranslation.begin());
        ImGui::DragFloat3("Scale",       modelScale.begin(), 0.1f);
        ImGui::DragFloat3("Rotation",    modelRotation.begin(), 0.03f);
        ImGui::ColorEdit4("Texture Color", color.begin());
    }

    void TestBatchedTextured::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
