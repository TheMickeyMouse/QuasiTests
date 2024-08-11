#pragma once
#include "CameraController.h"
#include "Light.h"
#include "Test.h"
#include "ModelLoading/OBJModel.h"

namespace Quasi::Graphics {
    struct MTLMaterial;
}

namespace Test {
    class TestLightCasters : public Test {
    public:
        struct Vertex {
            Math::fVector3 Position, Normal;
            int MaterialID;

            Q_GL_DEFINE_VERTEX(Vertex, 3D, (Position, Graphics::PosTf)(Normal, Graphics::NormTf)(MaterialID));
        };
    private:
        Graphics::RenderObject<Vertex> scene;
        Vec<Graphics::MTLMaterial> materials;
        Vec<Graphics::Mesh<Vertex>> meshes;
        Vec<Graphics::Light> lights;
        static constexpr int MAX_LIGHTS = 8;
        Graphics::CameraController camera;

        Graphics::RenderObject<Graphics::VertexColor3D> lightScene;
        Vec<Graphics::Mesh<Graphics::VertexColor3D>> lightMeshes;

        float ambientStrength = 0.03f, specularStrength = 1.2f;

        DEFINE_TEST_T(TestLightCasters, ADVANCED)
    public:
        TestLightCasters() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void UniformMaterial(const std::string& name, const Graphics::MTLMaterial& material);
        void UniformLight(const std::string& name, const Graphics::Light& light);

        void AddPointLight(const Graphics::PointLight& point, const Math::fColor& color);
    };
}
