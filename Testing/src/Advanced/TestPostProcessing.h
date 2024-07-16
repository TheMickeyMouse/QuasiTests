#pragma once
#include "FrameBuffer.h"
#include "Mesh.h"
#include "RenderBuffer.h"
#include "Test.h"
#include "Textures/Texture.h"

namespace Test {
    // all from https://learnopengl.com/Advanced-OpenGL/Framebuffers & https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
    class TestPostProcessing : public Test {
    private:
        Graphics::RenderObject<Graphics::VertexColor3D> scene;
        Graphics::RenderObject<Graphics::VertexTexture2D> postProcessingQuad;

        Graphics::FrameBuffer fbo;
        Graphics::Texture renderResult;
        Graphics::RenderBuffer depthStencilAttachment;

        Vec<Graphics::Mesh<Graphics::VertexColor3D>> cubes;
        Graphics::Mesh<Graphics::VertexTexture2D> screenQuad;

        Math::Transform3D transform;
        Math::Quaternion turnSpeed = 1;

        Math::fVector2 effectOff = 3;
        float hueShift = 0, satMul = 0, valShift = 0;

        Graphics::Shader shaderInv, shaderHsv, shaderBlur, shaderEdgeDetect, *currShader;

        bool usePostProcessing = true;

        DEFINE_TEST_T(TestPostProcessing, ADVANCED)
    public:
        TestPostProcessing() = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
