#include "DemoUIPlayground.h"

#include "imgui.h"
#include "GUI/ImGuiExt.h"

namespace Test {
    void DemoUIPlayground::OnInit(Graphics::GraphicsDevice& gdevice) {
        canvas = Graphics::Canvas { gdevice };
        Graphics::Render::SetClearColor(Math::fColor::LightGray());

        sprite = Graphics::Texture2D::LoadPNG(RES("sprite_minecraft.png"));

        button = Box<Clickable>::New({
            { {{ 300, 200 }, { 600, 300 }} },
            [&] (MouseEventType::E, IO::IO&) {
                // QInfo$("Click!");
                this->buttonColor = Math::fColor::Better::White();
            }
        });

        canvas.AddInteractable(button.AsRef());
        buttonColor = Math::fColor::Better::Lime();
    }

    void DemoUIPlayground::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        canvas.Update(deltaTime);
        buttonColor = buttonColor.Lerp(button->hovered ? Math::fColor::Better::Green() : Math::fColor::Better::Lime(), 0.1f);
    }

    void DemoUIPlayground::OnRender(Graphics::GraphicsDevice& gdevice) {
        canvas.BeginFrame();

        using namespace Graphics::UIRender;
        // canvas.StrokeCap((RenderStyle[]) { ROUND_CAP, SQUARE_CAP, ARROW_CAP, FLAT_CAP } [capMode]);
        // canvas.StrokeJoin((RenderStyle[]) { ROUND_JOIN, MITER_JOIN, BEVEL_JOIN } [joinMode]);
        //
        // canvas.StrokeWeight(24);
        // canvas.Stroke(Math::fColor::Better::Black());
        //
        // QWith$(p = canvas.NewPath(Graphics::Canvas::OPEN_CURVE)) {
        //     p.AddPoint(bezPoints[0]);
        //     p.AddQuadBez(bezPoints[1], bezPoints[2]);
        // }
        //
        // canvas.Fill(Math::fColor::Yellow());
        // canvas.DrawArc(
        //     { 300, 200 }, 150,
        //     Math::Degrees(30), Math::Degrees(gdevice.GetIO().Time.currentTime * 50),
        //     (Graphics::Canvas::ArcDirection)arcDirection, (Graphics::Canvas::ArcMode)arcMode
        // );
        //
        // canvas.DrawPoint({ 900, 700 });
        // canvas.DrawLine({ 500, 100 }, { 800, 300 });

        Graphics::UIMesh rsquare;
        QWith$(_ = canvas.RenderTo(rsquare)) {
            canvas.Fill(Math::fColor::White());
            canvas.DrawVarRoundedSquare({ 900, 200 }, 200, 60, 20, 20, 60);
        }
        rsquare.OverlayGradient(Graphics::Gradient::Vertical(400, 0, Math::fColor::Lime(), Math::fColor::Cyan()));
        canvas.DrawMesh(rsquare);

        canvas.Fill(buttonColor);
        canvas.DrawRoundedRect({{ 300, 200 }, { 600, 300 }}, 10);
        canvas.DrawText(text, size, { 300, 300 }, { .rect = { 300, 100 } });

        canvas.EndFrame();
    }

    void DemoUIPlayground::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::InputFloat("Text Size", &size);
        ImGui::EditString("String", text);
    }

    void DemoUIPlayground::OnDestroy(Graphics::GraphicsDevice& gdevice) {

    }
}
