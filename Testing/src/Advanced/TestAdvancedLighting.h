#pragma once
#include "CameraController.h"
#include "Test.h"
#include "ModelLoading/OBJModel.h"

namespace Test {
    class TestAdvancedLighting : public Test {
    public:
        struct Vertex {
            Math::fVector3 Position, Normal;
            int MaterialId;

            Q_GL_VERTEX_T(Vertex, 3D, (Position, Graphics::PosTf)(Normal, Graphics::NormTf)(MaterialId));
        };

    private:
        Graphics::RenderObject<Vertex> scene;

        Vec<Graphics::Mesh<Vertex>> meshes;
        Vec<Graphics::MTLMaterial> materials;
        Graphics::CameraController camera;

        Math::fVector3 lightPos = { 10, 27, 10 };
        Math::fColor3 lightColor = 1;
        float ambientStrength = 0.25f, specularStrength = 0.6f;

        DEFINE_TEST_T(TestAdvancedLighting, ADVANCED)
    public:
        TestAdvancedLighting() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;

        void UniformMaterial(Str name, const Graphics::MTLMaterial& material);
    };
}
