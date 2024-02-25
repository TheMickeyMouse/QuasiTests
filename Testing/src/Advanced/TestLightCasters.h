#pragma once
#include "CameraController.h"
#include "Light.h"
#include "Test.h"
#include "Model Loading/OBJModel.h"

namespace Test {
    class TestLightCasters : public Test {
    public:
        struct Vertex {
            Maths::fvec3 Position, Normal;
            int MaterialID;

            GL_VERTEX_T(Vertex);
            GL_VERTEX_FIELD((Position)(Normal)(MaterialID));
            GL_VERTEX_CUSTOM_TRANSFORM(mat) {
                return {
                    .Position = mat * Position,
                    .Normal = mat * Normal.with_w(0),
                    .MaterialID = MaterialID
                };
            }
        };
    private:
        Graphics::RenderObject<Vertex> scene;
        std::vector<Graphics::MTLMaterial> materials;
        std::vector<Graphics::Mesh<Vertex>> meshes;
        std::vector<Graphics::Light> lights;
        static constexpr int MAX_LIGHTS = 8;
        Graphics::CameraController camera;

        Graphics::RenderObject<VertexColor3D> lightScene;
        std::vector<Graphics::Mesh<VertexColor3D>> lightMeshes;

        float ambientStrength = 0.03f, specularStrength = 1.2f;

        DEFINE_TEST_T(TestLightCasters, ADVANCED)
    public:
        TestLightCasters() = default;
        ~TestLightCasters() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void UniformMaterial(const std::string& name, const Graphics::MTLMaterial& material);
        void UniformLight(const std::string& name, const Graphics::Light& light);

        void AddPointLight(const Graphics::PointLight& point, const Maths::colorf& color);
    };
}
