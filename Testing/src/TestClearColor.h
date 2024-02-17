#pragma once
#include "Test.h"

namespace Test {
    class TestClearColor : public Test {
    private:
        Maths::colorf clearColor = Maths::colorf::BETTER_BLUE();
    public:
        TestClearColor() {}
        ~TestClearColor() override {}

        OPENGL_API void OnRender(Graphics::GraphicsDevice& gdevice) override;
        OPENGL_API void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        OPENGL_API void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };
}
