#include "TestBatchedTextured.h"
#include "Graphicals/TriIndices.h"

#include "Utils/Extension/ImGuiExt.h"

namespace Test {
    void TestBatchedTextured::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Vertex>(8, 4);

        using namespace Graphics;
        textures[0] = Texture::LoadPNG(res("image.png").IntoCStr());
        textures[1] = Texture::LoadPNG(res("img1.png").IntoCStr(), { .params = { TextureParamPair::NearestSample() } });

        textures[0].Activate();
        textures[1].Activate();

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

        const int slots[] = { textures[0].Slot(), textures[1].Slot(), };
        render->shader.Bind();
        render->shader.SetUniformIntArr("u_textures", slots);
        render->shader.Unbind();
    }

    void TestBatchedTextured::OnRender(Graphics::GraphicsDevice& gdevice) {
        mesh.GeometryPass([&] (Vertex& v) { v.Color = color; });
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
