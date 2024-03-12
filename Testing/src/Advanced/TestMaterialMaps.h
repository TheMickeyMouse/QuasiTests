#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"
#include "Texture.h"

namespace Test {
    class TestMaterialMaps : Test {
    private:
        Graphics::RenderObject<VertexTextureNormal3D> scene;
        Graphics::RenderObject<VertexColor3D> lightScene;

        std::vector<Graphics::Mesh<VertexTextureNormal3D>> meshes;
        Graphics::Mesh<VertexColor3D> lightSource;
        Graphics::CameraController camera;

        Maths::fvec3 lightPos = { 0, 8, 2 };
        Maths::color3f lightColor = 1;
        float ambientStrength = 0.03f, specularStrength = 1.2f;
        Graphics::Texture diffuseMap, specularMap;

        DEFINE_TEST_T(TestMaterialMaps, ADVANCED)
    public:
        TestMaterialMaps() = default;
        ~TestMaterialMaps() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
};
