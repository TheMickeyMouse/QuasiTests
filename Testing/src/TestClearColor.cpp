#include "TestClearColor.h"

#include "Debugging.h"
#include <GL/glew.h>

#include "imgui.h"

namespace Test {
    void TestClearColor::OnRender(Graphics::GraphicsDevice& gdevice) {
        Test::OnRender(gdevice);
        gdevice.ClearColor(clearColor);
    }

    void TestClearColor::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        Test::OnImGuiRender(gdevice);
        ImGui::ColorEdit4("Clear Color", clearColor.begin());
    }

    void TestClearColor::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        Test::OnDestroy(gdevice);
        gdevice.ClearColor(0);
    }
}
