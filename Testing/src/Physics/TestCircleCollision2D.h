#pragma once
#include "Mesh.h"
#include "Test.h"
#include "Physics/World2D.h"

namespace Test {
    class TestCircleCollision2D : public Test {
        static constexpr u32 DYNAMIC_BALL_COUNT = 95,
                             STATIC_BALL_COUNT  = 5,
                             TOTAL_BALL_COUNT   = DYNAMIC_BALL_COUNT + STATIC_BALL_COUNT;

        using Vertex = Graphics::Vertex2D;
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> circleMesh;
        Graphics::Shader lineShader;
        Graphics::Mesh<Vertex> totalLineMesh;

        Physics2D::World world;
        Ref<const Physics2D::Body> edge[4] = {};

        Math::fRect2D viewport;

        Ref<Physics2D::Body> selected = nullptr;
        Math::fVector2 selectOffset = 0;
        Math::fVector2 lastDragPosition;

        DEFINE_TEST_T(TestCircleCollision2D, SIM_PHYSICS);
    public:
        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void AddRandomBall(Graphics::GraphicsDevice& gdevice);
        void ResetBalls(Graphics::GraphicsDevice& gdevice);
        [[nodiscard]] Ref<Physics2D::Body> FindBallAt(const Math::fVector2& mousePos) const;
    };
} // Test
