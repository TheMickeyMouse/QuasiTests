#pragma once
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class TestCubeRender : public Test {
    private:
        // unsigned int faceOrder[6] = { 0, 1, 2, 3, 4, 5 };
        Graphics::RenderObject<VertexColor3D> render;
        Graphics::Mesh<VertexColor3D> cube;

        Maths::mat3D projection = Maths::mat3D::ortho_projection({ -5.0f, 5.0f, -5.0f, 5.0f, -5.0f, 5.0f });
        Maths::fvec3 modelTranslation = 0;
        Maths::fvec3 modelScale       = 1;
        Maths::fvec3 modelRotation    = 0;
        float alpha = 1.0f;

        DEFINE_TEST_T(TestCubeRender, BASIC)
    public:
        TestCubeRender() {}
        ~TestCubeRender() override {}

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
