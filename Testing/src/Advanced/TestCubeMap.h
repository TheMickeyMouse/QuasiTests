#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"
#include "Textures/Texture.h"

namespace Test {
    class TestCubeMap : public Test {
        struct Vertex {
            Math::fVector3 Position, TextureCoordinate, Normal;

            Q_GL_DEFINE_VERTEX(Vertex, 3D, (Position, Graphics::PosTf)(TextureCoordinate)(Normal, Graphics::NormTf));
        };

        static constexpr u32 DIFFUSE_SHADER_ID = 0;
        static constexpr u32 REFLECTION_SHADER_ID = 1;
        static constexpr u32 REFRACTION_SHADER_ID = 2;
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

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
