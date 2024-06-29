#include "imgui.h"
#include "TestingFramework.h"

namespace Test {
    TestMenu::TestMenu(Ref<Test*> currTest) : currentTest(currTest) {
        testTypeSegments.resize((int)TestType::TOTAL, { TestType::OTHER, {}, "" });
    }

    void TestMenu::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (!ImGui::BeginTabBar("Testing Projects")) return;
        for (const auto& [type, span, desc] : testTypeSegments) {
            if (ImGui::BeginTabItem(ToDirString(type).data())) {
                ImGui::Text("%s", desc.data());
                for (usize j = span.min; j < span.max; ++j) {
                    const TestMenuItem& testItem = menuItems[j];
                    if (ImGui::Button(testItem.name.data())) {
                        // LOG("clicked " << testItem.name);
                        *currentTest = testItem.testConstruct();
                        currentTest.Value()->OnInit(gdevice);
                    }
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", testItem.description.data());
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    void TestMenu::DeclareTestType(TestType type) {
        const int t = (int)currentType;
        if (t >= 0 && t < (int)TestType::TYPE_MAX) {
            testTypeSegments[t].span.max = menuItems.size();
        }
        TestSection& section = testTypeSegments[(int)type];
        section.type = type;
        section.span.min = menuItems.size();
        currentType = type;
    }

    void TestMenu::FinishDeclare() {
        const int t = (int)currentType;
        testTypeSegments[t].span.max = menuItems.size();
    }

    void TestMenu::AddSectionDescription(const Str desc) {
        testTypeSegments[(int)currentType].description = desc;
    }

    void TestMenu::AddDescription(const Str desc) {
        menuItems.back().description = desc;
    }
}
