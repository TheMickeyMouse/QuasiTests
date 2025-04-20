#pragma once
#include "Test.h"
#include "Graphicals/Mesh.h"
#include "Utils/Fonts/Font.h"

namespace Test {
    class TestFontRender : public Test {
    public:
        struct Vertex {
            Math::fVector2 Position;
            Math::fColor   Color;
            Math::fVector2 TextureCoordinate;
            int isText = 0;
    
            QuasiDefineVertex$(Vertex, 2D, (Position, Graphics::PosTf)(Color)(TextureCoordinate)(isText));
        };
    private:
        Graphics::RenderObject<Vertex> render;
        Graphics::Mesh<Vertex> meshStr, meshAtlas, meshBg;
        bool showAtlas = false;
        int alignX = 0, alignY = 0, wrapMethod = 0;
        bool cropX = false, cropY = false;
        float letterSpace = 0, lineSpace = 1;
        Math::fRect2D textBox = { -200, 200, -200, 200 };

        float fontSize = 20;
        float thickness = 0.5f, softness = 0.05f;
        
        Math::fColor shadowColor = 0;
        float shadowSoftness = 0.3f;
        Math::fVector2 shadowOffset = { 5, 5 };

        String string = R"(Features include: <br>
New Line with `<br>`<br>
**bold text** with `**`<br>
*italic text* with `*`<br>
***bold & italic*** with `***`<br>
~~Strike trough like dis~~ with `~~`)";
        bool useMarkdown = true;

        Graphics::Font font;
        Math::fColor color = 1;

        Math::Matrix3D projection = Math::Matrix3D::ortho_projection({ -320.0f, 320.0f, -240.0f, 240.0f, -1.0f, 1.0f });
        Math::Transform2D transform;

        DEFINE_TEST_T(TestFontRender, ADVANCED)
    public:
        TestFontRender() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
