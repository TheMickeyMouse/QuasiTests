#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"
#include "Texture.h"

namespace Test {
    class TestCubeMap : Test {
    public:
        struct Vertex {
            Maths::fvec3 Position, TextureCoord, Normal;

            GL_VERTEX_T(Vertex);
            GL_VERTEX_FIELD((Position)(TextureCoord)(Normal));
            GL_VERTEX_TRANSFORM_FIELDS((Position)(Normal, Graphics::NormalTransformer));
        };
    private:
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> skybox, box;
        Graphics::CameraController camera;

        Graphics::Texture cubemap;
        Graphics::Texture boxTex;

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
