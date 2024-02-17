#include "TestTexturedSquare.h"

#include "imgui.h"

namespace Test {
    void TestTexturedSquare::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<VertexColorTexture3D>(4, 2);

        render.UseShader(Graphics::Shader::StdTextured);
        render.SetProjection(projection);
        
        texture = Graphics::Texture("res\\TestTexturedSquare\\image.png", false);
        texture.Activate();

        VertexColorTexture3D vertices[] = { 
            { { -200.0f, -200.0f, 0 }, 1, { 0.0f, 0.0f } },
            { { +200.0f, -200.0f, 0 }, 1, { 1.0f, 0.0f } },
            { { +200.0f, +200.0f, 0 }, 1, { 1.0f, 1.0f } },
            { { -200.0f, +200.0f, 0 }, 1, { 0.0f, 1.0f } },
        };

        Graphics::TriIndices indices[] = {
            { 0, 1, 2 },
            { 2, 3, 0 }
        };

        mesh = Graphics::Mesh(
            std::vector(vertices, vertices + 4),
            std::vector(indices, indices + 2)
        );

        render.BindMeshes(mesh);
    }

    void TestTexturedSquare::OnRender(Graphics::GraphicsDevice& gdevice) {
        Maths::mat3D mat = Maths::mat3D::transform(modelTranslation,
                                                   modelScale,
                                                   modelRotation);
        render.SetCamera(mat);
        render.GetShader().Bind();
        render.GetShader().SetUniformTex("u_Texture", texture);
        mesh.ApplyMaterial(&VertexColorTexture3D::Color, color);
        //LOG(mat);
        render.ResetData();
        render.Render();
    }

    void TestTexturedSquare::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::ColorEdit4("Texture Color", color.begin());
        ImGui::DragFloat3("Translation", modelTranslation.begin());
        ImGui::DragFloat3("Scale",       modelScale.begin(), 0.1f);
        ImGui::DragFloat3("Rotation",    modelRotation.begin(), 0.03f);
    }

    void TestTexturedSquare::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
