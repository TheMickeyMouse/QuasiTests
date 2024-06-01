#include "TestCircleCollision2D.h"

#include <imgui.h>

#include "Meshes/Circle.h"

namespace Test {
    void TestCircleCollision2D::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        viewport = { 0, 80, 0, 60 };

        using namespace Graphics::VertexBuilder;
        circleMesh = Graphics::MeshUtils::Circle({ 32 }, Vertex::Blueprint {
            .Position = GetPosition {}
        });

        scene.BindMeshes(circleMesh);

        scene.UseShaderFromFile(res("circle.vert"), res("circle.frag"));
        lineShader = Graphics::Shader::FromFile(res("line.vert"), res("line.frag"), res("line.geom"));

        scene.SetProjection(Maths::mat3D::ortho_projection({ 0, 80, 0, 60, -1, 1 }));

        world = { { 0, -40.0f }, Physics2D::Drag(0.95f, 60.0f) };
        ResetBalls(gdevice);

        auto& vert = totalLineMesh.GetVertices(); auto& inds = totalLineMesh.GetIndices();
        for (int i = 0; i < 6; ++i) vert.emplace_back(0.0f);
        inds.emplace_back(0, 1, 1);
        inds.emplace_back(1, 3, 3);
        inds.emplace_back(2, 3, 3);
        inds.emplace_back(0, 2, 2);
        inds.emplace_back(4, 5, 5);
    }

    void TestCircleCollision2D::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        const auto& mouse = gdevice.GetIO().Mouse;
        const Maths::fvec2 mousePos = mouse.GetMousePos().as<float>().map({ -1, 1, 1, -1 }, viewport);
        if (mouse.LeftOnPress() && !selected) {
            selected = FindBallAt(mousePos);
            if (selected)
                selectOffset = mousePos - selected->position;
        }

        if (mouse.LeftPressed() && selected) {
            const Maths::fvec2 newPos = mousePos - selectOffset;
            selected->position = newPos;
            selected->velocity = 0;
        }

        if (mouse.LeftOnRelease()) selected = nullptr;

        if (mouse.MiddleOnPress()) lastDragPosition = mousePos;
        if (mouse.MiddlePressed()) {
            for (const Physics2D::BodyHandle& circ : world.Bodies()) circ->position -= lastDragPosition - mousePos;
            lastDragPosition = mousePos;
        }

        if (mouse.RightOnPress() && !selected) {
            selected = FindBallAt(mousePos);
        }

        if (mouse.RightPressed() && selected) {
            totalLineMesh.GetVertices()[4].Position = selected->position;
            totalLineMesh.GetVertices()[5].Position = mousePos;
        }

        if (mouse.RightOnRelease() && selected && selected->IsDynamic()) {
            const bool scale = gdevice.GetIO().Keyboard.KeyPressed(IO::Key::LCONTROL);
            selected->velocity -= (scale ? 10.0f : 1.0f) * (mousePos - selected->position);
            totalLineMesh.GetVertices()[4].Position = 0;
            totalLineMesh.GetVertices()[5].Position = 0;
            selected = nullptr;
        }

        world.Update(deltaTime, 8, 2);

        for (int i = 0; i < 4; ++i) {
            const Physics2D::Body* e = edge[i];
            totalLineMesh.GetVertices()[i].Position = e->position + e->ShapeAs<Physics2D::EdgeShape>().start;
        }

        lineShader.Bind();
    }

    void TestCircleCollision2D::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.Shader().Bind();
        std::vector<Maths::fvec2> offsets;
        offsets.resize(100);
        std::ranges::transform(balls, offsets.begin(), [](const Physics2D::Body* b) { return b->position; });
        scene.Shader().SetUniformFvec2Arr("offsets", offsets);

        scene.ResetData();
        scene.RenderInstanced(100, {
            { "u_projection", scene.ProjectionMat() },
            { "selected", selected ? (int)(std::ranges::find(balls, selected) - balls.begin()) : -1 }
        }, false);

        scene.ClearData();
        scene.AddNewMeshes(totalLineMesh);
        scene.Render(lineShader, {
            { "u_projection", scene.ProjectionMat() }
        }, false);
    }

    void TestCircleCollision2D::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        if (ImGui::Button("Reset Balls")) ResetBalls(gdevice);

        ImGui::BulletText("Controls");
        ImGui::Text("Left Click = Drag");
        ImGui::Text("Right Click = Add Velocity");
        ImGui::Text(" + Control for increased Force");
        ImGui::Text("Middle Click = Move");

        ImGui::DragFloat2("Gravity", world.gravity.begin());
        ImGui::DragFloat("Inverse Drag", &world.drag, 0.005f, 0.0f, 1.0f);
    }

    void TestCircleCollision2D::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestCircleCollision2D::AddRandomBall(Graphics::GraphicsDevice& gdevice) {
        auto& rand = gdevice.GetRand();

        const Maths::fvec2 position = Maths::fvec2::random(rand, viewport);
        const float radius = rand.getf(1.0f, 3.0f);
        balls.emplace_back(world.CreateBody<Physics2D::CircleShape>({ .position = position, .density = 5.0f }, radius));
        colors.emplace_back(Maths::colorf::random(rand, rand.getf(0.3f, 0.7f), rand.getf(0.8f, 1.0f)));
    }

    void TestCircleCollision2D::ResetBalls(Graphics::GraphicsDevice& gdevice) {
        world.Clear();
        colors.clear();
        balls.clear();

        for (int i = 0; i < 99; ++i)
            AddRandomBall(gdevice);
        using namespace Physics2D;
        balls.emplace_back(world.CreateBody<CircleShape>({ .position = viewport.center(), .type = BodyType::STATIC, .density = 0.0f }, 5.0f));
        colors.emplace_back(0.5f);

        edge[0] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewport.corner(0), viewport.corner(1));
        edge[1] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewport.corner(1), viewport.corner(3));
        edge[2] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewport.corner(2), viewport.corner(0));
        edge[3] = world.CreateBody<EdgeShape>({ .type = BodyType::STATIC, .density = 0.0f }, viewport.corner(3), viewport.corner(2));

        scene.Shader().Bind();
        scene.Shader().SetUniformFvec4Arr("colors", stdu::span_cast<const Maths::fvec4>(std::span(colors)));
        for (int i = 0; i < 100; ++i)
            scene.Shader().SetUniformFloat(std::format("scales[{}]", i), balls[i]->ShapeAs<CircleShape>().radius);
    }

    Physics2D::Body* TestCircleCollision2D::FindBallAt(const Maths::fvec2& mousePos) const {
        for (Physics2D::Body* circ : balls) {
            if (circ->CollidesWith(Physics2D::CircleShape { 0.0f }, mousePos)) {
                return circ;
            }
        }
        return nullptr;
    }
} // Test