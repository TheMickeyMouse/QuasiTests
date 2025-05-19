#pragma once
#include <queue>

#include "Test.h"
#include "Mesh.h"
#include "Math/Geometry.h"
#include "Fonts/Font.h"

namespace Test {
    class DemoFlappyBird : public Test {
        struct Vertex {
            Math::fv2 Position;
            Math::fColor Color;
            Math::fv2 TextureCoord;
            int RenderType;

            QuasiDefineVertex$(Vertex, 2D, (Position, Graphics::PosTf)(Color)(TextureCoord)(RenderType));
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
