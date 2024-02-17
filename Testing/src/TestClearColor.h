#pragma once
#include "Test.h"

namespace Test {
    class TestClearColor : public Test {
    private:
        Maths::colorf clearColor = Maths::colorf::BETTER_BLUE();
    public:
        TestClearColor() {}
        ~TestClearColor() override {}

        void OnInit(Graphics::GraphicsDevice& gdevice) override {}
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
