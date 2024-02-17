#pragma once
#include "CameraController.h"
#include "Test.h"
#include "Model Loading/OBJModel.h"

namespace Test {
    class TestAdvancedLighting : Test {
    private:
        using Vertex = Graphics::OBJVertex;
        Graphics::RenderObject<Vertex> scene;

        Graphics::OBJModel model;
        Graphics::CameraController camera;
    public:
        TestAdvancedLighting() = default;
        ~TestAdvancedLighting() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
