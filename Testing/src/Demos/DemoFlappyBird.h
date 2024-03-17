#pragma once
#include <queue>

#include "Font.h"
#include "Geometry.h"
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class DemoFlappyBird : Test {
        struct Vertex {
            Maths::fvec2 Position;
            Maths::colorf Color;
            Maths::fvec2 TextureCoord;
            int RenderType;

            GL_VERTEX_T(Vertex);
            GL_VERTEX_FIELD((Position)(Color)(TextureCoord)(RenderType));
            GL_VERTEX_TRANSFORM_FIELDS((Position));
        };

        struct Spike {
            Graphics::Mesh<Vertex> mesh;
            Maths::Geometry::ftriangle2d collider;
            float xOff;
        };

        Graphics::RenderObject<Vertex> render;
        Graphics::Font font;
        Graphics::Mesh<Vertex> mPlayer, mText, mBg;
        std::deque<Spike> spikes;
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
