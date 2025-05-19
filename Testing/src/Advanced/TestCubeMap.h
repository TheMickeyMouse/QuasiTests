#pragma once
#include "../../../Quasi/src/Graphics/CameraController.h"
#include "../../../Quasi/src/Graphics/Mesh.h"
#include "Test.h"
#include "GLs/Texture.h"

namespace Test {
    class TestCubeMap : public Test {
        struct Vertex {
            Math::fv3 Position, TextureCoordinate, Normal;

            QuasiDefineVertex$(Vertex, 3D, (Position, Graphics::PosTf)(TextureCoordinate)(Normal, Graphics::NormTf));
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

        Math::Radians lightYaw = 0.61_rad, lightPitch = -0.979_rad;
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
