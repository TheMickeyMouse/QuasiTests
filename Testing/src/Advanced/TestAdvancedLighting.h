#pragma once
#include "Test.h"
#include "CameraController3D.h"
#include "ModelLoading/OBJModel.h"

namespace Test {
    class TestAdvancedLighting : public Test {
    public:
        struct Vertex {
            Math::fv3 Position, Normal;
            int MaterialId;

            QuasiDefineVertex$(Vertex, 3D, (Position, Position)(Normal, Normal)(MaterialId));
        };

    private:
        Graphics::RenderObject<Vertex> scene;

        Vec<Graphics::Mesh<Vertex>> meshes;
        Vec<Graphics::MTLMaterial> materials;
        Graphics::CameraController3D camera;

        Math::fv3 lightPos = { 10, 27, 10 };
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

        void UniformMaterial(u32 index, const Graphics::MTLMaterial& material);
    };
}
