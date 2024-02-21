#pragma once
#include "CameraController.h"
#include "Mesh.h"
#include "Test.h"
#include "Texture.h"

namespace Test {
    class TestMaterialMaps : Test {
    public:
        struct Vertex {
            Maths::fvec3 Position;
            Maths::fvec2 TextureCoordinate;
            Maths::fvec3 Normal;

            GL_VERTEX_T(Vertex);
            GL_VERTEX_FIELD((Position)(TextureCoordinate)(Normal));
            GL_VERTEX_CUSTOM_TRANSFORM(mat) {
                return {
                    .Position = mat * Position,
                    .TextureCoordinate = TextureCoordinate,
                    .Normal = mat * Normal.with_w(0)
                };
            }
        };
    private:
        Graphics::RenderObject<Vertex> scene;
        Graphics::RenderObject<VertexColor3D> lightScene;

        std::vector<Graphics::Mesh<Vertex>> meshes;
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
