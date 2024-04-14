#include "imgui.h"
#include "TestingFramework.h"

namespace Test {
    TestMenu::TestMenu(Test*& currTest) : currentTest(currTest) {
        testTypeSegments.resize((int)TestType::TOTAL, { TestType::OTHER, {}, "" });
    }

    void TestMenu::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (!ImGui::BeginTabBar("Testing Projects")) return;
        for (const auto& [type, span, desc] : testTypeSegments) {
            if (ImGui::BeginTabItem(ToDirString(type))) {
                ImGui::Text("%s", desc.c_str());
                for (usize j = span.min; j < span.max; ++j) {
                    const TestMenuItem& testItem = menuItems[j];
                    if (ImGui::Button(testItem.name.c_str())) {
                        // LOG("clicked " << testItem.name);
                        currentTest = testItem.testConstruct();
                        currentTest->OnInit(gdevice);
                    }
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", testItem.description.c_str());
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

    void TestMenu::AddSectionDescription(const std::string_view desc) {
        testTypeSegments[(int)currentType].description = desc;
    }

    void TestMenu::AddDescription(const std::string_view desc) {
        menuItems.back().description = desc;
    }


    void TestManager::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        deltaTime = std::isnan(deltaTime) ? gdevice.GetIO().Time.DeltaTimef() : deltaTime;
        if (currentTest)
            currentTest->OnUpdate(gdevice, deltaTime);
    }

    void TestManager::OnRender(Graphics::GraphicsDevice& gdevice) {
        if (currentTest)
            currentTest->OnRender(gdevice);
    }

    void TestManager::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (!currentTest) return;

        ImGui::Begin("Test");
        if (currentTest != menu.get() && ImGui::Button("<< Back")) {
            currentTest->OnDestroy(gdevice);
            delete currentTest;
            currentTest = menu.get();
            currentTest->OnInit(gdevice);
        }

        currentTest->OnImGuiRender(gdevice);

        ImGui::Separator();
        gdevice.DebugMenu();

        ImGui::End();
    }

    void TestManager::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        if (currentTest != menu.get())
            delete currentTest;
    }
}
