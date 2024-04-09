#pragma once
#include "CameraController.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Test.h"
#include "Texture.h"

namespace Test {
    class TestShadowMap : Test {
        using Vertex = Graphics::VertexColorNormal3D;
        Graphics::RenderObject<Vertex> scene;
        std::vector<Graphics::Mesh<Vertex>> meshes;
        Graphics::FrameBuffer depthMap;
        Graphics::Texture depthTex;
        Graphics::Shader depthShader;

        Graphics::RenderObject<Graphics::VertexTexture2D> shadowMapDisplay;
        Graphics::Mesh<Graphics::VertexTexture2D> screenQuad;

        Maths::fvec3 lightPosition { -8.2f, 11.2f, 6.6f };
        float ambStrength = 0.2f;
        Maths::rangef clipDistance = { 1.0f, 100.0f };
        bool showDepthMap = false, useSmoothShadows = true;

        Graphics::CameraController camera;

        DEFINE_TEST_T(TestShadowMap, ADVANCED);
    public:
        ~TestShadowMap() override = default;
        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
} // Test
