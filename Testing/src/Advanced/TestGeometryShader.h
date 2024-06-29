#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class TestGeometryShader : public Test {
        using Vertex = Graphics::VertexColorNormal3D;
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> sphere, icosphere;

        Graphics::Shader normalShader, flatShader;

        Graphics::CameraController camera;

        float lightYaw = 0.0f, lightPitch = 0.0f, ambStrength = 0.2f, normMag = 1.0f;
        Math::fColor normColor = Math::fColor::BETTER_WHITE();
        bool useGeomShader = true, useFlatShading = false;
        int displayFace = -1;

        DEFINE_TEST_T(TestGeometryShader, ADVANCED);
    public:

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
} // Test
