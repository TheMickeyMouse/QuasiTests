#include "TestClearColor.h"

#include "Debugging.h"
#include <GL/glew.h>

#include "imgui.h"

namespace Test {
    void TestClearColor::OnRender(Graphics::GraphicsDevice& gdevice) {
        gdevice.ClearColor(clearColor);
    }

    void TestClearColor::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::ColorEdit4("Clear Color", clearColor.begin());
    }

    void TestClearColor::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        gdevice.ClearColor(0);
    }
}
