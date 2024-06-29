#pragma once
#include "Test.h"

namespace Test {
    class TestStencilOutline : public Test {
        Graphics::RenderObject<Graphics::VertexColor3D> scene;
        Vec<Graphics::Mesh<Graphics::VertexColor3D>> meshes, outlinedMeshes;

        Math::fVector3 modelTranslation = { 0, 0, -3.5 };
        Math::fVector3 modelScale       = 1;
        Math::fVector3 modelRotation    = 0;

        Math::fVector3 turnSpeed = 1;
        Graphics::Shader outlineShader;
        Math::fColor outlineColor = 1;

        DEFINE_TEST_T(TestStencilOutline, ADVANCED)
    public:
        TestStencilOutline() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
