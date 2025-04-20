#pragma once
#include "Graphicals/CameraController.h"
#include "Graphicals/Mesh.h"
#include "Test.h"
#include "GLs/Texture.h"

namespace Test {
    class TestMaterialMaps : public Test {
    private:
        Graphics::RenderObject<Graphics::VertexTextureNormal3D> scene;
        Graphics::RenderObject<Graphics::VertexColor3D> lightScene;

        Vec<Graphics::Mesh<Graphics::VertexTextureNormal3D>> meshes;
        Graphics::Mesh<Graphics::VertexColor3D> lightSource;
        Graphics::CameraController camera;

        Math::fVector3 lightPos = { 0, 8, 2 };
        Math::fColor3 lightColor = 1;
        float ambientStrength = 0.03f, specularStrength = 1.2f;
        Graphics::Texture diffuseMap, specularMap;

        DEFINE_TEST_T(TestMaterialMaps, ADVANCED)
    public:
        TestMaterialMaps() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
};
