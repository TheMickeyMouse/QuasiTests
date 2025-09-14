#include "TestBatchedTextured.h"
#include "TriIndices.h"

#include "GUI/ImGuiExt.h"

namespace Test {
    void TestBatchedTextured::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Vertex>(8, 4);

        using namespace Graphics;
        textures[0] = Texture2D::LoadPNG(res("image.png").IntoCStr());
        textures[1] = Texture2D::LoadPNG(res("img1.png").IntoCStr(), { .pixelated = true });

        textures[0].Activate(1);
        textures[1].Activate(2);

        Vec<Vertex> vertices = Vec<Vertex>::New({
            { { -240.0f, -80.0f }, 1, { 0.0f, 0.0f }, 0 },
            { { -80.00f, -80.0f }, 1, { 1.0f, 0.0f }, 0 },
            { { -80.00f, +80.0f }, 1, { 1.0f, 1.0f }, 0 },
            { { -240.0f, +80.0f }, 1, { 0.0f, 1.0f }, 0 },

            { { +80.00f, -80.0f }, 1, { 0.0f, 0.0f }, 1 },
            { { +240.0f, -80.0f }, 1, { 1.0f, 0.0f }, 1 },
            { { +240.0f, +80.0f }, 1, { 1.0f, 1.0f }, 1 },
            { { +80.00f, +80.0f }, 1, { 0.0f, 1.0f }, 1 },
        });

        Vec<TriIndices> indices = Vec<TriIndices>::New({
            { 0, 1, 2 }, { 2, 3, 0 },
            { 4, 5, 6 }, { 6, 7, 4 }
        });

        mesh = Mesh(std::move(vertices), std::move(indices));

        render.SetProjection(projection);

        render.UseShaderFromFile(res("shader.vert").IntoCStr(), res("shader.frag").IntoCStr());

        constexpr int slots[] = { 1, 2 };
        render->shader.Bind();
        render->shader.SetUniformIntArr("u_textures", slots);
        render->shader.Unbind();
    }

    void TestBatchedTextured::OnRender(Graphics::GraphicsDevice& gdevice) {
        for (auto& v : mesh.vertices)
            v.Color = color;
        render.Draw(mesh);
    }

    void TestBatchedTextured::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditTransform("Transform", mesh.modelTransform, 0.1);
        ImGui::EditColor("Texture Color", color);
    }

    void TestBatchedTextured::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
