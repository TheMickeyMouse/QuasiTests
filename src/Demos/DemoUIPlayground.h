#pragma once
#include "Graphics/GUI/Canvas.h"
#include "Test.h"

namespace Test {
    class DemoUIPlayground : public Test {
        Graphics::Canvas canvas;
        float size = 20.0f;
        String text = "click";
        Graphics::Texture2D sprite;
        Math::fColor buttonColor;

        Math::fv2 bezPoints[3] = { { 200, 700 }, { 600, 1100 }, { 500, 400 } };

        Box<Interactable> button;

        DEFINE_TEST_T(DemoUIPlayground, DEMO)
    public:
        DemoUIPlayground() = default;
        ~DemoUIPlayground() override = default;

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
