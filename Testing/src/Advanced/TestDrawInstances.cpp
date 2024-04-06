#include "TestDrawInstances.h"

#include <imgui.h>

#include "Meshes/Circle.h"
#include "Meshes/Cube.h"

namespace Test {
    void TestDrawInstances::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();

        transforms.resize(INSTANCE_NUM);
        colors.resize(INSTANCE_NUM);

        using namespace Maths;
        const fvec4 black = color3f::BETTER_BLACK().as_rgbaf();
        const mat4x4 colorTransformer = {
            color3f::BETTER_RED().as_rgbaf() - black,
            color3f::BETTER_LIME().as_rgbaf() - black,
            color3f::BETTER_BLUE().as_rgbaf() - black,
            color3f::BETTER_BLACK().as_rgbaf()
        };
        for (uint i = 0; i < INSTANCE_NUM; ++i) {
            const uint x = i % 3, y = i / 3 % 3, z = i / 9;
            const fvec3 pos = { x, y, z };
            transforms[i].translation = (pos - 1) * 3;
            transforms[i].scale = 0.75f;
            colors[i] = (colorTransformer * (pos * pos / 4).with_w(1)).xyz().to_color3();
        }

        cube = Graphics::MeshUtils::Cube();
        scene.BindMeshes(cube);
        scene.UseShaderFromFile(res("instanced.vert"), res("instanced.frag"));

        camera.position = { -6.923308, -7.435342, -6.919785 };
        camera.yaw = -0.85972655; camera.pitch = 0.6728243;
        camera.speed = 5;
        camera.sensitivity = 0.12;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5;
        camera.smoothZoom = 120;
    }

    void TestDrawInstances::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestDrawInstances::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());

        scene.ResetData();

        std::vector<Maths::mat3D> modelMats, normMats;
        modelMats.resize(INSTANCE_NUM);
        normMats.resize(INSTANCE_NUM);
        std::ranges::transform(transforms, modelMats.begin(),
            [](const Transform& t) {
                return Maths::mat3D::transform(t.translation, t.scale, t.rotation);
            });
        std::ranges::transform(modelMats, normMats.begin(),
            [](const Maths::mat3D& m) {
                return m.inv().transpose();
            });

        scene.RenderInstanced(INSTANCE_NUM, {
            { "models",  std::span(std::as_const(modelMats)) }, // yeah c++ is just needy
            { "normMat", std::span(std::as_const(normMats)) },
            { "colors",  std::span(std::as_const(colors)) },
            { "lightDirection", Maths::fvec3::from_spheric(1, lightYaw, lightPitch) },
            { "ambientStrength", ambStrength },
        });
    }

    void TestDrawInstances::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::SliderFloat("Light Yaw", &lightYaw, -Maths::PI, Maths::PI);
        ImGui::SliderFloat("Light Pitch", &lightPitch, -Maths::HALF_PI * 0.95f, Maths::HALF_PI * 0.95f);
        ImGui::SliderFloat("Ambient", &ambStrength, 0.0f, 1.0f);
        if (ImGui::TreeNode("Cube Instances")) {
            for (uint i = 0; i < INSTANCE_NUM; ++i) {
                if (!ImGui::TreeNode(std::format("Cube #{}", i + 1).c_str())) continue;
                ImGui::DragFloat3("Translation", transforms[i].translation.begin());
                ImGui::DragFloat3("Scale", transforms[i].scale.begin(), 0.2f);
                ImGui::DragFloat3("Rotation", transforms[i].rotation.begin(), 0.05f);
                ImGui::ColorEdit3("Color", colors[i].begin());

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        if (ImGui::Button("Randomize Rotations")) RandomizeRotations(gdevice);

        camera.ImGuiEdit();
    }

    void TestDrawInstances::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestDrawInstances::RandomizeRotations(Graphics::GraphicsDevice& gdevice) {
        for (uint i = 0; i < INSTANCE_NUM; ++i) {
            transforms[i].rotation = Maths::fvec3::random(gdevice.GetRand(), { -Maths::PI, Maths::PI });
        }
    }
} // Test