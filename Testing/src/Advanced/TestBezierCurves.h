#pragma once
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class TestBezierCurves : public Test {
        struct BVertex {
            Math::fv2 Position;
            Math::fv2 BezCoord;
            Math::fColor Color;

            QuasiDefineVertex$(BVertex, 2D, (Position, Position)(BezCoord)(Color))
        };

        Graphics::RenderObject<BVertex> scene;
        Graphics::Mesh<BVertex> mesh;

        Math::fv2 bezierPoints[4];
        usize bezState = 0;

        DEFINE_TEST_T(TestBezierCurves, ADVANCED)
    public:
        TestBezierCurves() = default;
        ~TestBezierCurves() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
