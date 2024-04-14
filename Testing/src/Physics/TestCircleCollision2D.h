#pragma once
#include "Mesh.h"
#include "Test.h"
#include "Physics/CircleCollider2D.h"

namespace Test {
    class TestCircleCollision2D : Test {
        using Vertex = Graphics::Vertex2D;
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> circleMesh;
        Graphics::Shader lineShader;
        Graphics::Mesh<Vertex> totalLineMesh;

        std::vector<Physics2D::CircleCollider> circles;
        std::vector<Maths::colorf> colors;
        std::vector<float> scales;

        Maths::rect2f viewport;

        Physics2D::CircleCollider* selected = nullptr;
        Maths::fvec2 selectOffset = 0;
        Maths::fvec2 lastDragPosition;

        DEFINE_TEST_T(TestCircleCollision2D, SIM_PHYSICS);
    public:
        ~TestCircleCollision2D() override = default;
        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void AddRandomBall(Graphics::GraphicsDevice& gdevice);
        void ResetBalls(Graphics::GraphicsDevice& gdevice);
    };
} // Test
