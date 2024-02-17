#pragma once
#include <functional>

#include "GraphicsDevice.h"

namespace Test {
    class Test {
    public:
        virtual ~Test() = default;
        virtual void OnInit(Graphics::GraphicsDevice& gdevice) = 0;
        virtual void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) = 0;
        virtual void OnRender(Graphics::GraphicsDevice& gdevice) = 0;
        virtual void OnImGuiRender(Graphics::GraphicsDevice& gdevice) = 0;
        virtual void OnDestroy(Graphics::GraphicsDevice& gdevice) = 0;
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
        TestMenu(Test*& currTest);
        ~TestMenu() override = default;

        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;

        template <typename T>
        void RegisterTest(const std::string& name) {
            //LOG("Registered " << name << " Test");
            menuItems.emplace_back(name, []{ return (Test*)(new T()); });
        }

        void OnInit(Graphics::GraphicsDevice& gdevice) override {}
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override {}
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override {}
    };
}
