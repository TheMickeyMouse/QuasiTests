#pragma once

#include "GraphicsDevice.h"

#define DEFINE_TEST_T(T, S) \
    using _Test_t = T; \
    inline static const TestType _Test_Category_t = TestType::S; \
    inline static String res(Str resource) { \
        return std::format("{}res\\{}\\" #T "\\{}", PROJECT_DIRECTORY, _Test_Category_t->name, resource); \
    }

namespace Test {
    using namespace Quasi;

    constexpr Str PROJECT_DIRECTORY = { __FILE__, sizeof(__FILE__) - sizeof("src\\Test.h") };

    struct TestTypeData {
        Str name;

        Q_DEFINE_ENUM(TestType,
            (BASIC,       ("Basic"))
            (ADVANCED,    ("Advanced"))
            (SIM_PHYSICS, ("Physics"))
            (DEMO,        ("Demos"))
            (OTHER,       ("Other")),
        NULLABLE, ("None"))
    };

    class Test {
    public:
        virtual ~Test() = default;
        virtual void OnInit(Graphics::GraphicsDevice& gdevice) = 0;
        virtual void OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) = 0;
        virtual void OnRender(Graphics::GraphicsDevice& gdevice) = 0;
        virtual void OnImGuiRender(Graphics::GraphicsDevice& gdevice) = 0;
        virtual void OnDestroy(Graphics::GraphicsDevice& gdevice) = 0;
    };
}
