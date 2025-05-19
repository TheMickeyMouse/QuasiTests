#include "TestClearColor.h"

#include "GUI/ImGuiExt.h"

namespace Test {
    void TestClearColor::OnRender(Graphics::GraphicsDevice& gdevice) {
        gdevice.ClearColor(clearColor);
    }

    void TestClearColor::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditColor("Clear Color", clearColor);
    }

    void TestClearColor::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        gdevice.ClearColor(0);
    }
}
