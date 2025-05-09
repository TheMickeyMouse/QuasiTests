#pragma once
#include "Graphicals/CameraController.h"
#include "Graphicals/Mesh.h"
#include "Test.h"

namespace Test {
    class TestDrawInstances : public Test {
        using Vertex = Graphics::VertexNormal3D;
        static constexpr int INSTANCE_NUM = 27;
        Graphics::RenderObject<Vertex> scene;
        Graphics::Mesh<Vertex> cube;

        Vec<Math::Transform3D> transforms;
        Vec<Math::fColor3> colors;

        Math::Radians lightYaw = -0.346_rad, lightPitch = 0.088_rad;
        float ambStrength = 0.2f;

        Graphics::CameraController camera;

        DEFINE_TEST_T(TestDrawInstances, ADVANCED);
    public:
        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void RandomizeRotations(Graphics::GraphicsDevice& gdevice);
    };
} // Test
