#include "TestDrawInstances.h"

#include <imgui.h>

#include "VertexBlueprint.h"
#include "Extension/ImGuiExt.h"
#include "Meshes/Circle.h"
#include "Meshes/Cube.h"

namespace Test {
    void TestDrawInstances::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();

        transforms.resize(INSTANCE_NUM);
        colors.resize(INSTANCE_NUM);

        using namespace Math;
        const fVector4 black = fColor3::BETTER_BLACK().as_rgbaf();
        const Matrix4x4 colorTransformer = {
            fColor3::BETTER_RED().as_rgbaf()  - black,
            fColor3::BETTER_LIME().as_rgbaf() - black,
            fColor3::BETTER_BLUE().as_rgbaf() - black,
            fColor3::BETTER_BLACK().as_rgbaf()
        };
        for (u32 i = 0; i < INSTANCE_NUM; ++i) {
            const u32 x = i % 3, y = i / 3 % 3, z = i / 9;
            const fVector3 pos = { x, y, z };
            transforms[i].position = (pos - 1) * 3;
            transforms[i].scale = 0.75f;
            colors[i] = (colorTransformer * (pos * pos / 4).with_w(1)).xyz().to_color3();
        }

        cube = Graphics::MeshUtils::Cube(QGLCreateBlueprint$(Vertex, (
            in (Position, Normal),
            out (Position) = Position;,
            out (Normal) = Normal;
        )));

        scene.UseShaderFromFile(res("instanced.vert"), res("instanced.frag"));

        camera.position = { -6.8653593, -7.7674685, -6.846223 };
        camera.yaw = -2.2986794; camera.pitch = -0.55294377;
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

        Vec<Math::Matrix3D> modelMats, normMats;
        modelMats.reserve(INSTANCE_NUM);
        normMats.reserve(INSTANCE_NUM);
        for (const auto& t : transforms) {
            modelMats.push_back(t.TransformMatrix());
            normMats.push_back(t.NormalTransform().TransformMatrix());
        }

        scene.DrawInstanced(cube, INSTANCE_NUM, Graphics::UseArgs({
            { "models",         modelMats },
            { "normMat",        normMats },
            { "colors",         colors },
            { "lightDirection", Math::fVector3::from_spheric(1, lightYaw, lightPitch) },
            { "ambientStrength", ambStrength },
        }));
    }

    void TestDrawInstances::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditRotation("Light Rotation", lightYaw, lightPitch);
        ImGui::EditScalar("Ambient", ambStrength, 0.05f);
        if (ImGui::TreeNode("Cube Instances")) {
            for (u32 i = 0; i < INSTANCE_NUM; ++i) {
                if (!ImGui::TreeNode(std::format("Cube #{}", i + 1).c_str())) continue;
                ImGui::EditTransform("Transform", transforms[i], 0.2f);
                ImGui::EditColor("Color", colors[i]);

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        if (ImGui::Button("Randomize Rotations")) RandomizeRotations(gdevice);

        ImGui::EditCameraController("Camera", camera);
    }

    void TestDrawInstances::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestDrawInstances::RandomizeRotations(Graphics::GraphicsDevice& gdevice) {
        for (u32 i = 0; i < INSTANCE_NUM; ++i) {
            transforms[i].rotation = Math::Quaternion::random_rot(gdevice.GetRand());
        }
    }
} // Test