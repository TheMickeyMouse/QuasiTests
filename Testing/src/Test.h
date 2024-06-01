#pragma once

#include "GraphicsDevice.h"

#define DEFINE_TEST_T(T, S) \
    using _Test_t = T; \
    static constexpr TestType _Test_Category_t = TestType::S; \
    inline static std::string res(std::string_view resource) { \
        return std::format("{}res\\{}\\" #T "\\{}", PROJECT_DIRECTORY, ToDirString(_Test_Category_t), resource); \
    }

namespace Test {
    constexpr std::string_view PROJECT_DIRECTORY = { __FILE__, sizeof(__FILE__) - sizeof("src\\Test.h") };

    enum class TestType {
        BASIC,
        ADVANCED,
        SIM_PHYSICS,
        DEMO,
        OTHER,

        TYPE_MAX,
        TOTAL = TYPE_MAX,
    };

    inline STDU_ENUM_TOSTR(TestType, ToDirString,
        (BASIC, "Basic")(ADVANCED, "Advanced")(SIM_PHYSICS, "Physics")(DEMO, "Demos")(OTHER, "Others"), "")

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
