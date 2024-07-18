#include "imgui.h"
#include "TestMenu.h"

#include "TestManager.h"

namespace Test {
    TestMenu::TestMenu(Ref<TestManager> manager) : manager(manager) {
        testTypeSegments.resize(TestType::Num(), { TestType::OTHER, {}, "" });
    }

    void TestMenu::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (!ImGui::BeginTabBar("Testing Projects")) return;
        for (const auto& [type, span, desc] : testTypeSegments) {
            if (ImGui::BeginTabItem(type->name.data())) {
                ImGui::Text("%s", desc.data());
                for (usize j = span.min; j < span.max; ++j) {
                    const TestMenuItem& testItem = menuItems[j];
                    if (ImGui::Button(testItem.name.data())) {
                        // LOG("clicked " << testItem.name);
                        manager->testInstance.reset(testItem.testConstruct());
                        manager->currentTest = *manager->testInstance;
                        manager->currentTest->OnInit(gdevice);
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
        if (currentType)
            testTypeSegments[currentType.Ord()].span.max = menuItems.size();
        TestSection& section = testTypeSegments[type.Ord()];
        section.type = type;
        section.span.min = menuItems.size();
        currentType = type;
    }

    void TestMenu::FinishDeclare() {
        testTypeSegments[currentType.Ord()].span.max = menuItems.size();
    }

    void TestMenu::AddSectionDescription(const Str desc) {
        testTypeSegments[currentType.Ord()].description = desc;
    }

    void TestMenu::AddDescription(const Str desc) {
        menuItems.back().description = desc;
    }
}
