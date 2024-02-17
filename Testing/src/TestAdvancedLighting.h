#pragma once
#include "CameraController.h"
#include "Test.h"
#include "Graphics/Utils/Model Loading/OBJModel.h"

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

        OPENGL_API void OnInit(Graphics::GraphicsDevice& gdevice) override;
        OPENGL_API void OnUpdate(float deltaTime) override;
        OPENGL_API void OnRender(Graphics::GraphicsDevice& gdevice) override;
        OPENGL_API void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        OPENGL_API void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
