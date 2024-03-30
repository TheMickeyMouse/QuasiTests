#pragma once
#include "Test.h"

namespace Test {
    class TestStencilOutline : Test {
        Graphics::RenderObject<Graphics::VertexColor3D> scene;
        std::vector<Graphics::Mesh<Graphics::VertexColor3D>> meshes, outlinedMeshes;

        Maths::mat3D projection = Maths::mat3D::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f);
        Maths::fvec3 modelTranslation = { 0, 0, -3.5 };
        Maths::fvec3 modelScale       = 1;
        Maths::fvec3 modelRotation    = 0;

        Maths::fvec3 turnSpeed = 1;
        Graphics::Shader outlineShader;
        Maths::colorf outlineColor = 1;

        DEFINE_TEST_T(TestStencilOutline, ADVANCED)
    public:
        TestStencilOutline() = default;
        ~TestStencilOutline() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
