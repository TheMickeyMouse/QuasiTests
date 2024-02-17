#pragma once
#include "Test.h"
#include "Mesh.h"

namespace Test {
    class TestDynamicVertexGeometry : public Test {
    private:
        Graphics::RenderObject<VertexColor3D> render;
        Graphics::Mesh<VertexColor3D> mesh;

        Maths::mat3D projection = Maths::mat3D::ortho_projection({ -320.0f, 320.0f, -240.0f, 240.0f, -1.0f, 1.0f });

        DEFINE_TEST_T(TestDynamicVertexGeometry, BASIC)
    public:
        TestDynamicVertexGeometry() {}
        ~TestDynamicVertexGeometry() override {}

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
