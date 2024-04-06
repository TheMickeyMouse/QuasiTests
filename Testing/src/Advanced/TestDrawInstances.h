#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"

namespace Test {
    class TestDrawInstances : Test {
        struct Transform {
            Maths::fvec3 translation, scale, rotation;
        };

        using Vertex = Graphics::VertexNormal3D;
        static constexpr int INSTANCE_NUM = 27;
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> cube;

        std::vector<Transform> transforms;
        std::vector<Maths::color3f> colors;

        float lightYaw = -2.136f, lightPitch = -0.262f, ambStrength = 0.2f;

        Graphics::CameraController camera;

        DEFINE_TEST_T(TestDrawInstances, ADVANCED);
    public:
        ~TestDrawInstances() override = default;
        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void RandomizeRotations(Graphics::GraphicsDevice& gdevice);
    };
} // Test
