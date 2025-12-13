#pragma once

#include "TestMenu.h"

namespace Test {
    class TestManager {
    private:
        Graphics::GraphicsDevice gdevice;

        Box<Test> testInstance = nullptr;
        Box<TestMenu> menu;
        OptRef<Test> currentTest;
    public:
        TestManager();

        void OnInit();
        void OnUpdate(float deltaTime = f32s::NAN);
        void OnRender();
        void OnImGuiRender();
        void OnDestroy();
        void OnRun();

        bool WindowIsOpen() const { return gdevice.WindowIsOpen(); }

        friend class TestMenu;
    };
}
