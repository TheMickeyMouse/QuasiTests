#include "TestCircleCollision2D.h"

#include <imgui.h>

#include "Extension/ImGuiExt.h"
#include "Meshes/Circle.h"

namespace Test {
    void TestCircleCollision2D::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        viewport = { 0, 80, 0, 60 };

        using namespace Graphics::VertexBuilder;
        circleMesh = Graphics::MeshUtils::Circle({ 32 }, Vertex::Blueprint {
            .Position = GetPosition {}
        });

        scene.UseShaderFromFile(res("circle.vert"), res("circle.frag"));
        lineShader = Graphics::Shader::FromFile(res("line.vert"), res("line.frag"), res("line.geom"));

        scene.SetProjection(Math::Matrix3D::ortho_projection({ 0, 80, 0, 60, -1, 1 }));

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
        const Math::fVector2 mousePos = mouse.GetMousePos().as<float>().map({ -1, 1, 1, -1 }, viewport);
        if (mouse.LeftOnPress() && !selected) {
            selected = FindBallAt(mousePos);
            if (selected)
                selectOffset = mousePos - selected->position;
        }

        if (mouse.LeftPressed() && selected) {
            const Math::fVector2 newPos = mousePos - selectOffset;
            selected->position = newPos;
            selected->velocity = 0;
        }

        if (mouse.LeftOnRelease()) selected = nullptr;

        if (mouse.MiddleOnPress()) lastDragPosition = mousePos;
        if (mouse.MiddlePressed()) {
            for (auto circ : world.bodies) circ->position -= lastDragPosition - mousePos;
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

        world.Update(deltaTime, 16);

        for (int i = 0; i < 4; ++i) {
            Ref<const Physics2D::Body> e = edge[i];
            totalLineMesh.vertices[2 * i + 0].Position = e->position + e->shape.As<Physics2D::EdgeShape>()->start;
            totalLineMesh.vertices[2 * i + 1].Position = e->position + e->shape.As<Physics2D::EdgeShape>()->end;
        }

        lineShader.Bind();
    }

    void TestCircleCollision2D::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene->shader.Bind();
        Math::fVector2 offsets[TOTAL_BALL_COUNT];
        float scales[TOTAL_BALL_COUNT];
        Math::fColor colors[TOTAL_BALL_COUNT];

        const Math::fRange xRange = {
            world.bodies.front()->ComputeBoundingBox().min.x,
            world.bodies.back() ->ComputeBoundingBox().max.x
        };
        usize i = 0;
        int selectedIndex = -1;
        for (const auto body : world.bodies) {
            if (body->shape.As<Physics2D::EdgeShape>()) continue;
            if (selectedIndex == -1 && body.RefEquals(selected)) selectedIndex = (int)i;
            offsets[i] = body->position;
            scales[i]  = body->shape.As<Physics2D::CircleShape>()->radius;
            colors[i]  = Math::fColor::from_hsv(
                Math::Unit { offsets[i].x }.map(xRange, { 0, 360.f }).value(),
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

        const Math::fVector2 position = Math::fVector2::random(rand, viewport.inset(1.0f));
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
                .position = Math::fVector2::random(rand, viewport),
                .type = BodyType::STATIC,
                .density = 0.0f },
            rand.Get(4.0f, 6.0f));
        }

        Math::fRect2D viewExt = viewport.extrude(1.0f);

        edge[0] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewExt.corner(0), viewExt.corner(1), 1.0f);
        edge[1] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewExt.corner(1), viewExt.corner(3), 1.0f);
        edge[2] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewExt.corner(2), viewExt.corner(0), 1.0f);
        edge[3] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewExt.corner(3), viewExt.corner(2), 1.0f);
    }

    Ref<Physics2D::Body> TestCircleCollision2D::FindBallAt(const Math::fVector2& mousePos) const {
        for (auto circ : world.bodies) {
            if (circ->OverlapsWith(Physics2D::CircleShape { 0.0f }, mousePos)) {
                return circ;
            }
        }
        return nullptr;
    }
} // Test