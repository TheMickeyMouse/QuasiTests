#pragma once
#include "Graphicals/Mesh.h"
#include "Test.h"

namespace Test {
    class TestCubeRender : public Test {
    private:
        // unsigned int faceOrder[6] = { 0, 1, 2, 3, 4, 5 };
        Graphics::RenderObject<Graphics::VertexColor3D> render;
        Graphics::Mesh<Graphics::VertexColor3D> cube;

        Math::Matrix3D projection = Math::Matrix3D::ortho_projection({ -5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f });
        float alpha = 1.0f;

        DEFINE_TEST_T(TestCubeRender, BASIC)
    public:
        TestCubeRender() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
