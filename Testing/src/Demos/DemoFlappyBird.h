#pragma once
#include <queue>

#include "Geometry.h"
#include "Mesh.h"
#include "Test.h"
#include "Fonts/Font.h"

namespace Test {
    class DemoFlappyBird : public Test {
        struct Vertex {
            Math::fVector2 Position;
            Math::fColor Color;
            Math::fVector2 TextureCoord;
            int RenderType;

            QGLDefineVertex$(Vertex, 2D, (Position, Graphics::PosTf)(Color)(TextureCoord)(RenderType));
        };

        struct Spike {
            Graphics::Mesh<Vertex> mesh;
            Math::fTriangle2D collider;
            float xOff;
        };

        Graphics::RenderObject<Vertex> render;
        Graphics::Font font;
        Graphics::Mesh<Vertex> mPlayer, mText, mBg;
        Vec<Spike> spikes;
        u32 loopStart = 0;

        float velocityY = 0, yPos = 0;
        double time = 0, nextSpawnTime = 0;

        int score = 0;
        bool isEnd = false;

        DEFINE_TEST_T(DemoFlappyBird, DEMO)
    public:
        DemoFlappyBird() = default;
        ~DemoFlappyBird() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void ManageSpikes(Graphics::GraphicsDevice& gdevice);
        void CheckPlayerCollisions();
    };
}
