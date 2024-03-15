#pragma once
#include "CameraController.h"
#include "Test.h"
#include "OBJModel.h"

namespace Test {
    class TestAdvancedLighting : Test {
    public:
        struct Vertex {
            Maths::fvec3 Position, Normal;
            int MaterialId;

            GL_VERTEX_T(Vertex);
            GL_VERTEX_FIELD((Position)(Normal)(MaterialId));
            GL_VERTEX_CUSTOM_TRANSFORM(mat) {
                return {
                    .Position = mat * Position,
                    .Normal = mat.inv().transpose() * Normal.with_w(0),
                    .MaterialId = MaterialId
                };
            }
        };

    private:
        Graphics::RenderObject<Vertex> scene;

        std::vector<Graphics::Mesh<Vertex>> meshes;
        std::vector<Graphics::MTLMaterial> materials;
        Graphics::CameraController camera;

        Maths::fvec3 lightPos = { 10, 27, 10 };
        Maths::color3f lightColor = 1;
        float ambientStrength = 0.25f, specularStrength = 0.6f;

        DEFINE_TEST_T(TestAdvancedLighting, ADVANCED)
    public:
        TestAdvancedLighting() = default;
        ~TestAdvancedLighting() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void UniformMaterial(const std::string& name, const Graphics::MTLMaterial& material);
    };
}
