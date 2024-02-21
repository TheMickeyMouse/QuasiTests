#pragma once

#include <functional>

#include "Test.h"
#include "Basic/TestClearColor.h"
#include "Basic/TestBatchedTextured.h"
#include "Basic/TestDynamicVertexGeometry.h"
#include "Basic/TestDynamicQuadGeometry.h"
#include "Basic/TestCubeRender.h"

#include "Advanced/TestFontRender.h"
#include "Advanced/TestAdvancedLighting.h"
#include "Advanced/TestPostProcessing.h"
#include "Advanced/TestMaterialMaps.h"

#include "Demos/DemoFlappyBird.h"

namespace Test {
    class TestMenu : public Test {
    public:
        struct TestMenuItem {
            std::string name, description;
            std::function<Test*()> testConstruct;
        };


        struct TestSection {
            TestType type;
            Maths::urange span;
            std::string description;
        };

        static STDU_ENUM_TOSTR(TestType, TypeNameOf, (BASIC, "Basic")(ADVANCED, "Advanced")(DEMO, "Demo"), "Other")  // NOLINT(clang-diagnostic-reserved-identifier)
    private:
        Test*& currentTest; // ref bc currtest could be a testmenu (this)  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        std::vector<TestMenuItem> menuItems;
        std::vector<TestSection> testTypeSegments;
        TestType currentType = (TestType)-2;
    public:
        TestMenu(Test*& currTest);
        ~TestMenu() override = default;

        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;

        template <typename T>
        void RegisterTest(const std::string& name) {
            //LOG("Registered " << name << " Test");
            menuItems.emplace_back(name, "", []{ return (Test*)(new T()); });
        }

        void DeclareTestType(TestType type);
        void FinishDeclare();
        void AddSectionDescription(std::string_view desc);
        void AddDescription(std::string_view desc);

        void OnInit(Graphics::GraphicsDevice& gdevice) override {}
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override {}
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override {}
    };

    class TestManager : public Test {
    private:
        Test* currentTest = nullptr;
        std::unique_ptr<TestMenu> menu = std::make_unique<TestMenu>(currentTest);
    public:
        TestManager() {}
        ~TestManager() override {}

        void OnInit(Graphics::GraphicsDevice& gdevice) override;
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime = NAN) override;
        void OnRender(Graphics::GraphicsDevice& gdevice) override;
        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override;
    };

    inline void TestManager::OnInit(Graphics::GraphicsDevice& gdevice) {
        currentTest = menu.get();
        {
            menu->DeclareTestType(TestType::BASIC);
            menu->AddSectionDescription("Simple OpenGL Tests to make sure the renderer is working. \nTests mostly involve: Texture Rendering and Vertex manipulation");

            menu->RegisterTest<TestClearColor>           ("Clear Color Test        ");
            menu->AddDescription("Clears the screen with a color.");

            menu->RegisterTest<TestBatchedTextured>      ("Texture Batching Test   ");
            menu->AddDescription("Draws 2 textured squares.");

            menu->RegisterTest<TestDynamicVertexGeometry>("Dyn Vertex Geometry Test");
            menu->AddDescription("Draws 2 Modifiable squares.");

            menu->RegisterTest<TestDynamicQuadGeometry>  ("Dyn Quad Geometry Test  ");
            menu->AddDescription("Draws up to 8 Unique Modifiable squares.");

            menu->RegisterTest<TestCubeRender>           ("Cube 3D Rendering Test  ");
            menu->AddDescription("Draws a 3D cube.");

            // =========================================================================

            menu->DeclareTestType(TestType::ADVANCED);
            menu->AddSectionDescription("Advanced OpenGL Tests focused on more realistic rendering. \nIncluding: Text Rendering, Post Processing Effects and Lighting");

            menu->RegisterTest<TestFontRender>      ("Font Rendering Test     ");
            menu->AddDescription("Draws text (not 100% finished yet: markdown).");

            menu->RegisterTest<TestPostProcessing>  ("Post Processing Test    ");
            menu->AddDescription("Draws a simple scene with post processing.");

            menu->RegisterTest<TestAdvancedLighting>("Advanced Lighting Test  ");
            menu->AddDescription("Draws a simple scene with lighting.");

            menu->RegisterTest<TestMaterialMaps>    ("Material Map Test  ");
            menu->AddDescription("Draws materials with texture maps.");

            // =========================================================================

            menu->DeclareTestType(TestType::DEMO);
            menu->AddSectionDescription("Simple OpenGL 'Games' created to explore the limitations of the engine. \nJust to see what's possible without over-development.");

            menu->RegisterTest<DemoFlappyBird>("Flappy Bird Demo        ");
            menu->AddDescription("A dumb-down version of Flappy Bird.");

            menu->DeclareTestType(TestType::OTHER);
            menu->AddSectionDescription("Other Tests that didn't fit in. \nThere's nothing here yet!");

            menu->FinishDeclare();
        }
    }
}
