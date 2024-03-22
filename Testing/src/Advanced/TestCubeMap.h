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

        static constexpr int DIFFUSE_SHADER_ID = 0;
        static constexpr int REFLECTION_SHADER_ID = 1;
        static constexpr int REFRACTION_SHADER_ID = 2;
    private:
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> skybox, box;
        Graphics::CameraController camera;

        Graphics::Texture cubemap, boxTex;
        Graphics::Shader cubemapShader, boxShader, reflectShader, refractShader;

        float lightYaw = -2.532f, lightPitch = 0.979f;
        float ambStrength = 0.2f, refractiveIndex = 1.52f;
        int shaderID = 0;

        DEFINE_TEST_T(TestCubeMap, ADVANCED)
    public:
        TestCubeMap() = default;
        ~TestCubeMap() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
