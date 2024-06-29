#pragma once
#include "Test.h"
#include "Mesh.h"
#include "Textures/Texture.h"

namespace Test {
    class TestBatchedTextured : public Test {
    public:
        struct Vertex {
            Math::fVector3  Position;
            Math::fColor Color;
            Math::fVector2  TextureCoordinate;
            int TextureID;

            GL_VERTEX_T(Vertex);
            GL_VERTEX_FIELD((Position)(Color)(TextureCoordinate)(TextureID));
            GL_VERTEX_TRANSFORM_FIELDS((Position))
        };
    private:
        Graphics::RenderObject<Vertex> render;
        Graphics::Mesh<Vertex> mesh;
        Graphics::Texture textures[2];

        Math::fColor color = 1;

        Math::Matrix3D projection = Math::Matrix3D::ortho_projection({ -320.0f, 320.0f, -240.0f, 240.0f, -1.0f, 1.0f });
        Math::fVector3 modelTranslation = 0;
        Math::fVector3 modelScale = 1;
        Math::fVector3 modelRotation = 0;

        DEFINE_TEST_T(TestBatchedTextured, BASIC)
    public:
        TestBatchedTextured() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
