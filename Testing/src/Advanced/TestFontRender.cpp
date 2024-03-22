#include "TestFontRender.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "lambdas.h"
#include "VertexConverter.h"
#include "Graphics/Utils/Fonts/Font.h"
#include "stdu/rich_string.h"

namespace Test {
    void TestFontRender::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Vertex>(1024, 1024);

        render.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        render.SetProjection(projection);

        font = Graphics::Font::LoadFile(res("arial.ttf"));
        font.AddDefaultFontStyle(res("arialbd.ttf"), Graphics::FontStyle::BOLD);
        font.AddDefaultFontStyle(res("ariali.ttf"),  Graphics::FontStyle::ITALIC);
        font.AddDefaultFontStyle(res("arialbi.ttf"), Graphics::FontStyle::BOLD_ITALIC);
        font.AddMonoFontStyle(res("JetBrainsMono-Regular.ttf"));
        font.AddMonoFontStyle(res("JetBrainsMono-Bold.ttf"),       Graphics::FontStyle::BOLD);
        font.AddMonoFontStyle(res("JetBrainsMono-Italic.ttf"),     Graphics::FontStyle::ITALIC);
        font.AddMonoFontStyle(res("JetBrainsMono-BoldItalic.ttf"), Graphics::FontStyle::BOLD_ITALIC);

        font.SetSize(48);
        font.RenderBitmap();
        font.GetTexture().Activate();

        const Maths::fvec2 size = font.GetTexture().Size2D();
        const float x = size.x / size.y;
        Vertex atlVertices[] = { 
            { { -100.0f * x, -100.0f, 0 }, 1, { 0.0f, 1.0f }, 1 },
            { { +100.0f * x, -100.0f, 0 }, 1, { 1.0f, 1.0f }, 1 },
            { { +100.0f * x, +100.0f, 0 }, 1, { 1.0f, 0.0f }, 1 },
            { { -100.0f * x, +100.0f, 0 }, 1, { 0.0f, 0.0f }, 1 },
        };
        
        Graphics::TriIndices atlIndices[] = {
            { 0, 1, 2 },
            { 2, 3, 0 }
        };
        
        meshAtlas = Graphics::Mesh(
            std::vector(atlVertices, atlVertices + 4),
            std::vector(atlIndices, atlIndices + 2)
        );

        constexpr Maths::colorf bgColor = Maths::colorf::BETTER_BLACK();
        Vertex bgVertices[] = { 
            { { -200.0f, -200.0f, 0 }, bgColor, { 0.0f, 0.0f }, 0 },
            { { +200.0f, -200.0f, 0 }, bgColor, { 0.0f, 0.0f }, 0 },
            { { +200.0f, +200.0f, 0 }, bgColor, { 0.0f, 0.0f }, 0 },
            { { -200.0f, +200.0f, 0 }, bgColor, { 0.0f, 0.0f }, 0 },
        };
        
        Graphics::TriIndices bgIndices[] = {
            { 0, 1, 2 },
            { 2, 3, 0 }
        };
        
        meshBg = Graphics::Mesh(
            std::vector(bgVertices, bgVertices + 4),
            std::vector(bgIndices, bgIndices + 2)
        );

        render.BindMeshes(meshBg);
    }

    void TestFontRender::OnRender(Graphics::GraphicsDevice& gdevice) {
        const Maths::mat3D mat = Maths::mat3D::transform(modelTranslation,
                                                   modelScale,
                                                   modelRotation);
        render.SetCamera(mat);

        auto& vert = meshBg.GetVertices();
        vert[0].Position = textBox.corner(0);
        vert[1].Position = textBox.corner(1);
        vert[2].Position = textBox.corner(3); // yes this is correct
        vert[3].Position = textBox.corner(2);
        
        render.ResetData();
        if (showAtlas) {
            render.AddNewMeshes(meshAtlas);
        } else {
            using namespace Graphics::VertexBuilder;
            using FontVertex = Graphics::Font::Vertex;
            meshStr.Replace(font.RenderRichText(
                    stdu::rich_string::parse_markdown(string),
                    Graphics::PointPer64::inP64((int)(fontSize * 64.0f)),
                    Graphics::TextAlign { textBox }
                    .Align({ alignX, alignY << 2, wrapMethod << 4, cropX << 6, cropY << 7 })
                    .SpaceOut(lineSpace, Graphics::PointPer64::inP64((int)(letterSpace * 64.0f)))
                ).Convert<Vertex>(Vertex::Blueprint {
                    .Position = GetPosition {},
                    .Color = FromArg<&FontVertex::RenderType, &FontVertex::Color>(λ(int r, const Maths::colorf& col, r ? color : col)),
                    .TextureCoordinate = GetTextureCoord {},
                    .isText = Get<&FontVertex::RenderType> {}
                }));
            render.AddNewMeshes(meshStr);
        }
        render.Render({
            { "u_font",           font.GetTexture() },
            { "u_thickness",      thickness },
            { "u_softness",       softness },
            { "u_shadowColor",    shadowColor },
            { "u_shadowSoftness", shadowSoftness },
            { "u_shadowOffset",   shadowOffset },
        });
    }

    void TestFontRender::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Translation", modelTranslation.begin());
        ImGui::DragFloat3("Scale",       modelScale.begin(), 0.1f);
        ImGui::DragFloat3("Rotation",    modelRotation.begin(), 0.03f);

        ImGui::InputTextMultiline("String", &string);
        
        ImGui::ColorEdit4("Color",       color.begin());
        ImGui::SliderFloat("Font Size",   &fontSize, 4, 48);
        ImGui::SliderFloat("Thickness",   &thickness, 0, 1);
        ImGui::SliderFloat("Softness",    &softness,  0, 1);

        ImGui::SliderFloat2("Text Bottom Left", textBox.min.begin(), -300, 300);
        ImGui::SliderFloat2("Text Top Right",   textBox.max.begin(), -300, 300);

        ImGui::Combo("Alignment X", &alignX, "Center\0Left\0Right\0Justified\0\0");
        ImGui::Combo("Alignment Y", &alignY, "Center\0Top\0Bottom\0Justified\0\0");
        ImGui::Combo("Wrap Mode", &wrapMethod, "None\0Break Sentence\0Break Words\0\0");
        ImGui::Checkbox("Crop X", &cropX); ImGui::SameLine(); ImGui::Checkbox("Crop Y", &cropY);
        ImGui::SliderFloat("Letter Spacing", &letterSpace, -10, 10);
        ImGui::SliderFloat("Line   Spacing", &lineSpace, -1, 2);

        if (ImGui::CollapsingHeader("Shadow Properties")) {
            ImGui::ColorEdit4("Shadow Color", shadowColor.begin());
            ImGui::SliderFloat("Shadow Softness", &shadowSoftness, 0, 1);
            ImGui::SliderFloat2("Shadow Offset", shadowOffset.begin(), -10, 10);
        }

        if (ImGui::Button(showAtlas ? "Hide Atlas" : "Show Atlas")) {
            showAtlas ^= true;
        }

        if (showAtlas) {
            const char searchChar = string.back();
            const float x = meshAtlas.GetVertices()[1].Position.x;
            const auto& glyph = font.GetGlyphRect(searchChar);
            meshAtlas.SetTransform(Maths::mat3D::translate_mat({ x * (1 - 2 * glyph.rect.center().x), 0, 0 }));
        }
    }

    void TestFontRender::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
