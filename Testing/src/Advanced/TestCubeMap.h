#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"
#include "Texture.h"

namespace Test {
    class TestCubeMap : Test {
    public:
        using Vertex = Graphics::VertexTextureNormal3D;
    private:
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> skybox;
        Graphics::CameraController camera;

        Graphics::Texture cubemap;

        DEFINE_TEST_T(TestCubeMap, ADVANCED)
    public:
        ~TestCubeMap() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
