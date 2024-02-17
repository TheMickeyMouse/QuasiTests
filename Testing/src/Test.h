#pragma once
#include <functional>

#include "GraphicsDevice.h"

namespace Test {
    class Test {
    public:
        Test() {}
        virtual ~Test() {}

        virtual void OnInit(Graphics::GraphicsDevice& gdevice) {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(Graphics::GraphicsDevice& gdevice) {}
        virtual void OnImGuiRender(Graphics::GraphicsDevice& gdevice) {}
        virtual void OnDestroy(Graphics::GraphicsDevice& gdevice) {}
    };

    class TestMenu : public Test {
        struct TestMenuItem {
            std::string name;
            std::function<Test*()> testConstruct;
        };
    private:
        Test*& currentTest; // ref bc currtest could be a testmenu (this)  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        std::vector<TestMenuItem> menuItems;
    public:
        OPENGL_API TestMenu(Test*& currTest);

        OPENGL_API void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;

        template <typename T>
        void RegisterTest(const std::string& name) {
            //LOG("Registered " << name << " Test");
            menuItems.emplace_back(name, []{ return (Test*)(new T()); });
        }
    };
}
