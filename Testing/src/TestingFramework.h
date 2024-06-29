#pragma once

#include "Test.h"

namespace Test {
    class TestMenu : public Test {
    public:
        struct TestMenuItem {
            Str name, description;
            FnPtr<Test*> testConstruct;
        };

        struct TestSection {
            TestType type;
            Math::zRange span;
            Str description;
        };

    private:
        Ref<Test*> currentTest;
        Vec<TestMenuItem> menuItems;
        Vec<TestSection> testTypeSegments;
        TestType currentType = (TestType)-2;
    public:
        TestMenu(Ref<Test*> currTest);

        void OnImGuiRender(Graphics::GraphicsDevice& gdevice) override;

        template <class T>
        static Test* TestFactory() { return new T(); }

        template <typename T>
        void RegisterTest(Str name) {
            // LOG("Registered " << name << " Test");
            menuItems.emplace_back(name, "", TestFactory<T>);
        }

        void DeclareTestType(TestType type);
        void FinishDeclare();
        void AddSectionDescription(Str desc);
        void AddDescription(Str desc);

        void OnInit(Graphics::GraphicsDevice& gdevice) override {}
        void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) override {}
        void OnRender(Graphics::GraphicsDevice& gdevice) override {}
        void OnDestroy(Graphics::GraphicsDevice& gdevice) override {}
    };

    class TestManager {
    private:
        Test* currentTest = nullptr;
        UniqueRef<TestMenu> menu = NewUnique<TestMenu>(currentTest);
        Graphics::GraphicsDevice gdevice;
    public:
        TestManager() : gdevice(Graphics::GraphicsDevice::Initialize({ 1200, 900 })) {}

        void OnInit();
        void OnUpdate(float deltaTime = NAN);
        void OnRender();
        void OnImGuiRender();
        void OnDestroy();
        void OnRun();

        [[nodiscard]] bool WindowIsOpen() const { return gdevice.WindowIsOpen(); }
    };
}
