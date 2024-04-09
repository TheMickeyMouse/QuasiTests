#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class TestGeometryShader : Test {
        using Vertex = Graphics::VertexColorNormal3D;
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> sphere, icosphere;

        Graphics::Shader normalShader, flatShader;

        Graphics::CameraController camera;

        float lightYaw = 0.0f, lightPitch = 0.0f, ambStrength = 0.2f, normMag = 1.0f;
        Maths::colorf normColor = Maths::colorf::BETTER_WHITE();
        bool useGeomShader = true, useFlatShading = false;
        int displayFace = -1;

        DEFINE_TEST_T(TestGeometryShader, ADVANCED);
    public:
        ~TestGeometryShader() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
} // Test
