#include "TestCircleCollision2D.h"

#include <imgui.h>

#include "GLs/VertexBlueprint.h"
#include "GUI/ImGuiExt.h"
#include "Meshes/Circle.h"

#include "Utils/Iter/MapIter.h"

namespace Test {
    void TestCircleCollision2D::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        viewport = { 0, { 80, 60 } };

        circleMesh = Graphics::MeshUtils::Circle({ 32 }, QGLCreateBlueprint$(Vertex, (
            in (Position),
            out (Position) = Position;
        )));

        scene.UseShaderFromFile(res("circle.vert").IntoCStr(), res("circle.frag").IntoCStr());
        lineShader = Graphics::Shader::FromFile(res("line.vert").IntoCStr(), res("line.frag").IntoCStr(), res("line.geom").IntoCStr());

        scene.SetProjection(Math::Matrix3D::OrthoProjection({ { 0, 0, -1 }, { 80, 60, 1 } }));

        world = { { 0, -40.0f } };
        ResetBalls(gdevice);

        auto meshp = totalLineMesh.NewBatch();
        for (int i = 0; i < 10; ++i)
            meshp.PushV({ 0.0f });
        meshp.PushI(0, 1, 1);
        meshp.PushI(2, 3, 3);
        meshp.PushI(4, 5, 5);
        meshp.PushI(6, 7, 7);
        meshp.PushI(8, 9, 9);
    }

    void TestCircleCollision2D::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        const auto& mouse = gdevice.GetIO().Mouse;
        const Math::fv2 mousePos = mouse.GetMousePos().As<float>().MapCoords({ { -1, 1 }, { 1, -1 } }, viewport);
        if (mouse.LeftOnPress() && !selected) {
            selected = FindBallAt(mousePos);
            if (selected)
                selectOffset = mousePos - selected->position;
        }

        if (mouse.LeftPressed() && selected) {
            const Math::fv2 newPos = mousePos - selectOffset;
            selected->position = newPos;
            selected->velocity = 0;
        }

        if (mouse.LeftOnRelease()) selected = nullptr;

        if (mouse.MiddleOnPress()) lastDragPosition = mousePos;
        if (mouse.MiddlePressed()) {
            for (auto& circ : world.bodies) circ->position -= lastDragPosition - mousePos;
            lastDragPosition = mousePos;
        }

        if (mouse.RightOnPress() && !selected) {
            selected = FindBallAt(mousePos);
            selected = selected && selected->IsDynamic() ? selected : nullptr;
        }

        if (mouse.RightPressed() && selected) {
            totalLineMesh.vertices[8].Position = selected->position;
            totalLineMesh.vertices[9].Position = mousePos;
        }

        if (mouse.RightOnRelease() && selected && selected->IsDynamic()) {
            const bool scale = gdevice.GetIO().Keyboard.KeyPressed(IO::Key::LCONTROL);
            selected->velocity -= (scale ? 10.0f : 1.0f) * (mousePos - selected->position);
            totalLineMesh.vertices[8].Position = 0;
            totalLineMesh.vertices[9].Position = 0;
            selected = nullptr;
        }

        {
            // [[maybe_unused]] Timer t { "PhysicsUpdate", Debug::Logger::InternalLog };
            world.Update(deltaTime, 32);
        }

        for (int i = 0; i < 4; ++i) {
            auto r = edge[i]->shape.As<Physics2D::RectShape>();
            totalLineMesh.vertices[2 * i + 0].Position = r->Corner(i == 0, i == 2) + edge[i]->position;
            totalLineMesh.vertices[2 * i + 1].Position = r->Corner(i != 1, i != 3) + edge[i]->position;
        }

        lineShader.Bind();
    }

    void TestCircleCollision2D::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene->shader.Bind();
        Array<Math::fv2, TOTAL_BALL_COUNT> offsets;
        Array<float, TOTAL_BALL_COUNT> scales;
        Array<Math::fColor, TOTAL_BALL_COUNT> colors;

        const fRange xRange = fRange::Over(
            world.bodies.Iter().Map([] (const Box<Physics2D::Body>& x) { return x->position.x; })
        );
        usize i = 0;
        int selectedIndex = -1;
        for (auto& body : world.bodies) {
            if (body->shape.Is<Physics2D::RectShape>()) continue;
            if (selectedIndex == -1 && selected && selected.RefEquals(body.AsRef())) selectedIndex = (int)i;
            offsets[i] = body->position;
            scales[i] = body->shape.As<Physics2D::CircleShape>()->radius;
            colors[i] = Math::fColor::FromHSV(
                xRange.MapTo(offsets[i].x, { 0, 1 }),
                body->IsStatic() ? 0.2f : 0.8f,
                body->IsStatic() ? 0.5f : 0.8f);
            ++i;
        }

        scene.DrawInstanced(circleMesh, TOTAL_BALL_COUNT, Graphics::UseArgs({
            { "u_projection", scene->projection },
            { "selected",     selectedIndex },
            { "offsets",      offsets },
            { "scales",       scales },
            { "colors",       colors }
        }, false));

        scene.Draw(totalLineMesh, UseShaderWithArgs(lineShader, {
            { "u_projection", scene->projection }
        }, false));
    }

    void TestCircleCollision2D::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (ImGui::Button("Reset Balls")) ResetBalls(gdevice);

        ImGui::BulletText("Controls");
        ImGui::Text("Left Click = Drag");
        ImGui::Text("Right Click = Add Velocity");
        ImGui::Text(" + Control for increased Force");
        ImGui::Text("Middle Click = Move");

        ImGui::EditVector("Gravity", world.gravity);
    }

    void TestCircleCollision2D::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestCircleCollision2D::AddRandomBall(Graphics::GraphicsDevice& gdevice) {
        auto& rand = gdevice.GetRand();

        const Math::fv2 position = Math::fv2::Random(rand, viewport.Inset(3.0f));
        const float radius = rand.Get(1.0f, 3.0f);
        world.CreateBody<Physics2D::CircleShape>({ .position = position, .density = 5.0f }, radius);
    }

    void TestCircleCollision2D::ResetBalls(Graphics::GraphicsDevice& gdevice) {
        world.Clear();

        for (u32 i = 0; i < DYNAMIC_BALL_COUNT; ++i)
            AddRandomBall(gdevice);

        using namespace Physics2D;
        auto& rand = gdevice.GetRand();

        for (u32 i = 0; i < STATIC_BALL_COUNT; ++i) {
            world.CreateBody<CircleShape>({
                .position = fv2::Random(rand, viewport),
                .type = BodyType::STATIC,
                .density = 0.0f },
            rand.Get(4.0f, 6.0f));
        }

        edge[0] = world.CreateBody<RectShape>({ .position = {                0, +viewport.Height() * 0.5f }, .type = BodyType::STATIC }, 1.0f, viewport.Height() * 0.5f - 1.0f);
        edge[1] = world.CreateBody<RectShape>({ .position = { viewport.Width(), +viewport.Height() * 0.5f }, .type = BodyType::STATIC }, 1.0f, viewport.Height() * 0.5f - 1.0f);
        edge[2] = world.CreateBody<RectShape>({ .position = { viewport.Width() * 0.5f, +                0 }, .type = BodyType::STATIC }, viewport.Width() * 0.5f - 1.0f,  1.0f);
        edge[3] = world.CreateBody<RectShape>({ .position = { viewport.Width() * 0.5f, +viewport.Height() }, .type = BodyType::STATIC }, viewport.Width() * 0.5f - 1.0f,  1.0f);
    }

    OptRef<Physics2D::Body> TestCircleCollision2D::FindBallAt(const Math::fv2& mousePos) {
        const Physics2D::Shape mouse = Physics2D::CircleShape { 0.0f };
        for (auto& circ : world.bodies) {
            if (circ->OverlapsWith(mouse, mousePos)) {
                return *circ;
            }
        }
        return nullptr;
    }
} // Test