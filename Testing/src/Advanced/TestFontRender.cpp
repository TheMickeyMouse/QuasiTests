#include "TestFontRender.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "RichString.h"
#include "VertexConverter.h"
#include "Textures/Texture.h"

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

        const Math::fVector2 size = font.GetTexture().Size2D();
        const float x = size.x / size.y;
        Vec<Vertex> atlVertices = {
            { { -100.0f * x, -100.0f }, 1, { 0.0f, 1.0f }, 1 },
            { { +100.0f * x, -100.0f }, 1, { 1.0f, 1.0f }, 1 },
            { { +100.0f * x, +100.0f }, 1, { 1.0f, 0.0f }, 1 },
            { { -100.0f * x, +100.0f }, 1, { 0.0f, 0.0f }, 1 },
        };
        
        Vec<Graphics::TriIndices> atlIndices = {
            { 0, 1, 2 },
            { 2, 3, 0 }
        };
        
        meshAtlas = Graphics::Mesh(std::move(atlVertices), std::move(atlIndices));

        const Math::fColor bgColor = Math::fColor::BETTER_BLACK();
        Vec<Vertex> bgVertices = {
            { { -200.0f, -200.0f }, bgColor, { 0.0f, 0.0f }, 0 },
            { { +200.0f, -200.0f }, bgColor, { 0.0f, 0.0f }, 0 },
            { { +200.0f, +200.0f }, bgColor, { 0.0f, 0.0f }, 0 },
            { { -200.0f, +200.0f }, bgColor, { 0.0f, 0.0f }, 0 },
        };
        
        Vec<Graphics::TriIndices> bgIndices = {
            { 0, 1, 2 },
            { 2, 3, 0 }
        };
        
        meshBg = Graphics::Mesh(std::move(bgVertices), std::move(bgIndices));
    }

    void TestFontRender::OnRender(Graphics::GraphicsDevice& gdevice) {
        const Math::Matrix3D mat = Math::Matrix3D::transform(modelTranslation,
                                                   modelScale,
                                                   modelRotation);
        render.SetCamera(mat);

        auto& vert = meshBg.vertices;
        vert[0].Position = textBox.corner(0);
        vert[1].Position = textBox.corner(1);
        vert[2].Position = textBox.corner(3); // yes this is correct
        vert[3].Position = textBox.corner(2);

        if (!showAtlas) {
            using namespace Graphics::VertexBuilder;
            using FontVertex = Graphics::Font::Vertex;

            Graphics::TextAlign alignment =
                Graphics::TextAlign { textBox }
                .Align({ alignX, alignY << 2, wrapMethod << 4, cropX << 6, cropY << 7 })
                .SpaceOut(lineSpace, Graphics::PointPer64::inP64((int)(letterSpace * 64.0f)));

            meshStr = (useMarkdown ?
                font.RenderRichText(Text::RichString::ParseMarkdown(string), fontSize, alignment) :
                font.RenderText(string, fontSize, alignment)
            ).GeometryMap<Vertex>(Vertex::Blueprint {
                .Position = GetPosition {},
                .Color = FromArg<&FontVertex::RenderType, &FontVertex::Color>([&] (int r, const Math::fColor& col) { return r ? color : col; }),
                .TextureCoordinate = GetTextureCoord {},
                .isText = Get<&FontVertex::RenderType> {}
            });
        }
        render.Draw({ &meshBg, showAtlas ? &meshAtlas : &meshStr }, Graphics::UseArgs({
            { "u_font",           font.GetTexture() },
            { "u_thickness",      thickness },
            { "u_softness",       softness },
            { "u_shadowColor",    shadowColor },
            { "u_shadowSoftness", shadowSoftness },
            { "u_shadowOffset",   shadowOffset },
        }));
    }

    void TestFontRender::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Translation", modelTranslation.begin());
        ImGui::DragFloat3("Scale",       modelScale.begin(), 0.1f);
        ImGui::DragFloat3("Rotation",    modelRotation.begin(), 0.03f);

        ImGui::InputTextMultiline("String", &string);
        ImGui::Checkbox("Parse as Markdown", &useMarkdown);
        
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
            const float x = meshAtlas.vertices[1].Position.x;
            const auto& glyph = font.GetGlyphRect(string.back());
            meshAtlas.SetTransform(Math::Transform2D::Translation({ x * (1 - 2 * glyph.rect.center().x), 0 }));
        }
    }

    void TestFontRender::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
