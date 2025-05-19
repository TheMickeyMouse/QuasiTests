#pragma once
#include "Test.h"
#include "../../../Quasi/src/Graphics/Mesh.h"

namespace Test {
    class TestDynamicVertexGeometry : public Test {
        using Vertex = Graphics::VertexColor2D;
    private:
        Graphics::RenderObject<Vertex> render;
        Graphics::Mesh<Vertex> mesh;

        Math::Matrix3D projection = Math::Matrix3D::OrthoProjection({ { -320, -240, -1 }, { 320, 240, 1 } });

        DEFINE_TEST_T(TestDynamicVertexGeometry, BASIC)
    public:
        TestDynamicVertexGeometry() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
