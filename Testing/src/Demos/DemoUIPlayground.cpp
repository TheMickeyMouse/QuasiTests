#include "DemoUIPlayground.h"

#include "imgui.h"

namespace Test {
    void DemoUIPlayground::OnInit(Graphics::GraphicsDevice& gdevice) {
        canvas = Graphics::Canvas { gdevice };
        Graphics::Render::SetClearColor(Math::fColor::LightGray());
    }

    void DemoUIPlayground::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {

    }

    void DemoUIPlayground::OnRender(Graphics::GraphicsDevice& gdevice) {
        canvas.BeginFrame();

        using namespace Graphics::UIRender;
        canvas.StrokeCap((RenderStyle[]) { ROUND_CAP, SQUARE_CAP, ARROW_CAP, FLAT_CAP } [capMode]);
        canvas.StrokeJoin((RenderStyle[]) { ROUND_JOIN, MITER_JOIN, BEVEL_JOIN } [joinMode]);

        canvas.StrokeWeight(8);
        canvas.Stroke(Math::fColor::Better::Black());
        // canvas.StrokeJoin(Graphics::UIRender::MITER_JOIN);
        canvas.Fill(Math::fColor::Blue());
        canvas.DrawTriangle({ 600, 450 }, { 450, 600 }, { 750, 800 });

        canvas.Fill(Math::fColor::Red());
        canvas.DrawCircle({ 200, 600 }, 150);

        canvas.Fill(Math::fColor::Yellow());
        canvas.DrawArc(
            { 300, 200 }, 150,
            Math::Degrees(30), Math::Degrees(gdevice.GetIO().Time.currentTime * 50),
            (Graphics::Canvas::ArcDirection)arcDirection, (Graphics::Canvas::ArcMode)arcMode
        );

        canvas.Fill(Math::fColor::Lime());
        canvas.DrawRoundedRect({ { 800, 200 }, { 1000, 500 } }, 20);

        canvas.EndFrame();
    }

    void DemoUIPlayground::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::Combo("Cap Mode",  &capMode,  "Round\0Square\0Arrow\0Flat\0");
        ImGui::Combo("Join Mode", &joinMode, "Round\0Miter\0Bevel\0");
        ImGui::Combo("Arc Direction", &arcDirection, "Counter Clockwise\0Clockwise\0Major\0Minor\0");
        ImGui::Combo("Arc Mode",      &arcMode,      "Open\0Chord\0Closed\0");
    }

    void DemoUIPlayground::OnDestroy(Graphics::GraphicsDevice& gdevice) {

    }
}
