#include "DemoFlappyBird.h"

#include <imgui.h>
#include "GLs/VertexBlueprint.h"
#include "Utils/Meshes/Circle.h"
#include "Utils/Meshes/Quad.h"
#include "Utils/Iter/MapIter.h"

namespace Test {
    void DemoFlappyBird::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Vertex>(128, 128);
        render.UseShaderFromFile(res("shader.vert").IntoCStr(), res("shader.frag").IntoCStr());
        render.SetProjection(Math::Matrix3D::ortho_projection({ -320.0f, 320.0f, -240.0f, 240.0f, -1.0f, 1.0f }));

        font = Graphics::Font::LoadFile(res("consola.ttf").IntoCStr());
        font.SetSize(48);
        font.RenderBitmap();
        font.GetTexture().Activate(0);

        mPlayer = Graphics::MeshUtils::Circle({ 32 }, QGLCreateBlueprint$(Vertex, (
                in (Position),
                out (Position) = Position;,
                out (Color)    = Math::fColor::BETTER_WHITE();
            )), Math::Transform2D::Scaling(30.0f));

        const auto blueprint = QGLCreateBlueprint$(Vertex, (
            in (Position),
            out (Position) = Position;,
            out (Color)    = Math::fColor::BETTER_GREEN();
        ));

        mBg = Graphics::Mesh<Vertex>::Combine(Spans::Vals({
            Graphics::MeshUtils::Quad(blueprint, Math::Transform2D { { 0, +240 }, { 320, 20 } }),
            Graphics::MeshUtils::Quad(blueprint, Math::Transform2D { { 0, -240 }, { 320, 20 } })
        }));

        time = gdevice.GetIO().Time.currentTime;
        nextSpawnTime = 0;
        Graphics::Render::SetClearColor(Math::fColor::BETTER_BLACK());
    }

    void DemoFlappyBird::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        if (!isEnd) {
            if (gdevice.GetIO().Keyboard.KeyOnPress(IO::Key::SPACE)) {
                velocityY = 250;
            }

            yPos += velocityY * deltaTime;
            velocityY -= 400 * deltaTime;
            score = (int)(gdevice.GetIO().Time.currentTime - time);
        }

        ManageSpikes(gdevice);
        CheckPlayerCollisions();
    }

    void DemoFlappyBird::OnRender(Graphics::GraphicsDevice& gdevice) {
        render->shader.Bind();
        render->shader.SetUniformTex("u_font", font.GetTexture());

        using namespace Graphics;
        mPlayer.SetTransform(Math::Transform2D::Translation({ -150, yPos }));
        mText = font.RenderText(Text::Format("{}", score), 80,
            TextAlign { { -20, 20, 100, 140 } }.SpaceOut(1, -16))
            .GeometryMap<Vertex>([](const Font::Vertex& v) -> Vertex {
                return { v.Position, v.Color, v.TextureCoord, 1 };
        });

        render.BeginContext();
        render.AddMeshes({ &mPlayer, &mBg, &mText });
        render.AddMeshes(spikes.Iter().Map(Operators::Member<&Spike::mesh> {}));
        render.EndContext();
        render.DrawContext();
    }

    void DemoFlappyBird::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::Text("Spike Count: %zu", spikes.Length());
    }

    void DemoFlappyBird::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
        Graphics::Render::SetClearColor(0);
    }

    void DemoFlappyBird::ManageSpikes(Graphics::GraphicsDevice& gdevice) {
        if (isEnd) return;
        nextSpawnTime += gdevice.GetIO().Time.DeltaTime();
        if (nextSpawnTime > 2) {
            nextSpawnTime -= 2;
            const float midY = gdevice.GetRand().Get(-100.0f, 100.0f);
            Math::fTriangle2D top = { { -50,  220 }, { 50,  220 }, { gdevice.GetRand().Get(-15.0f, 15.0f), midY + 90 } };
            Math::fTriangle2D bot = { { -50, -220 }, { 50, -220 }, { gdevice.GetRand().Get(-15.0f, 15.0f), midY - 90 } };

            spikes.Push({ {
                { { top.p1, Math::fColor::BETTER_LIME(), 0, 0 }, { top.p2, Math::fColor::BETTER_LIME(), 0, 0 }, { top.p3, Math::fColor::BETTER_LIME(), 0, 0 } },
                { { 0, 1, 2 } }
            }, top, 370.0f });
            spikes.Push({ {
                { { bot.p1, Math::fColor::BETTER_LIME(), 0, 0 }, { bot.p2, Math::fColor::BETTER_LIME(), 0, 0 }, { bot.p3, Math::fColor::BETTER_LIME(), 0, 0 } },
                { { 0, 1, 2 } }
            }, bot, 370.0f });
        }

        for (Spike& spike : spikes) {
            spike.xOff -= 150 * gdevice.GetIO().Time.DeltaTime();
            spike.mesh.SetTransform(Math::Transform2D::Translation({ spike.xOff, 0 }));
        }

        if (spikes && spikes.First().xOff <= -370.0f) {
            spikes.Erase(0, 2);
        }
    }

    void DemoFlappyBird::CheckPlayerCollisions() {
        if (yPos <= -190.0f || 190.0f <= yPos) goto die; // NOLINT(cppcoreguidelines-avoid-goto, hicpp-avoid-goto)
        for (const Spike& spike : spikes) {
            for (int i = 1; i < 3; ++i) {
                const Math::fLine2D  line = spike.collider.line(i) + Math::fVector2::unit_x(spike.xOff);
                const Math::fVector2 center   = Math::fVector2 { -150, yPos },
                                     toCenter = center - line.start,
                                     dist     = line.forward(),
                                     proj     = toCenter.projected(dist);
                const float          factor   = std::abs(dist.x) > std::abs(dist.y) ? proj.x / dist.x : proj.y / dist.y;
                if (center.distsq(factor <= 0 ? line.start : (factor >= 1 ? line.end : proj + line.start)) <= 30.0f * 30.0f)
                    goto die;
            }
        }
        return;

        die:
        isEnd = true;
        mPlayer.GeometryPass([] (Vertex& v) { v.Color = Math::fColor::BETTER_RED(); });
    }
}
