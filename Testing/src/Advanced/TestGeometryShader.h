#pragma once
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class TestGeometryShader : Test {
        Graphics::RenderObject<Graphics::VertexColor3D> scene;
        Graphics::Mesh<Graphics::VertexColor3D> meshes;

        DEFINE_TEST_T(TestGeometryShader, ADVANCED);
    public:
        ~TestGeometryShader() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
} // Test
