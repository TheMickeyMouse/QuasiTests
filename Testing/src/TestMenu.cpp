#include "imgui.h"
#include "Test.h"

namespace Test {
    TestMenu::TestMenu(Test*& currTest) : currentTest(currTest) {}

    void TestMenu::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        for (TestMenuItem& testItem : menuItems) {
            if ( ImGui::Button(testItem.name.c_str()) ) {
                // LOG("clicked " << testItem.name);
                currentTest = testItem.testConstruct();
                currentTest->OnInit(gdevice);
            }
        }
    }
}