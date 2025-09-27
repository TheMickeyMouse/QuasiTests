#include "TestGeometryShader.h"

#include <imgui.h>

#include "GLs/VertexBlueprint.h"
#include "GUI/ImGuiExt.h"
#include "Meshes/Arrow3D.h"
#include "Meshes/Capsule.h"
#include "Meshes/Cube.h"
#include "Meshes/Icosphere.h"
#include "Meshes/Sphere.h"

namespace Test {
    void TestGeometryShader::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(8192, 4096);

        Graphics::Render::EnableCullFace();
        Graphics::Render::SetCullFace(Graphics::FacingMode::BACK);
        Graphics::Render::SetFrontFacing(Graphics::OrientationMode::COUNTER_CLOCKWISE);

        using namespace Math;

        fColor objectColor;
        const auto blueprint = QGLCreateBlueprint$(Vertex, (
            in (Position, Normal),
            out (Position) = Position;,
            out (Color)    = objectColor;,
            out (Normal)   = Normal;,
        ));

        objectColor = fColor::Better::Aqua();
        Graphics::Meshes::Sphere(10.0f, 20, 20).Merge(blueprint,
            Matrix3D::Translation({ 30, 0, 0 }).AsTransform(), meshes.NewBatch());

        objectColor = fColor::Better::Red();
        Graphics::Meshes::Icosphere(10.0f, 2).Merge(blueprint,
            Matrix3D::Translation({ 15, 0, 26 }).AsTransform(), meshes.NewBatch());

        objectColor = fColor::Better::Lime();
        Graphics::Meshes::Capsule({ 0, -7, 0 }, { 0, 7, 0 }, 7.0f, 20, 10).Merge(blueprint,
            Matrix3D::Translation({ -15, 0, 26 }).AsTransform(), meshes.NewBatch());

        objectColor = fColor::Better::Magenta();
        Graphics::Meshes::Cube().Merge(blueprint,
            Matrix3D::Transform({ -30, 0, 0 }, { 8.0f }, {}).AsTransform(), meshes.NewBatch());

        objectColor = fColor::Better::Orange();
        Graphics::Meshes::Arrow3D({ -18, -10, -29 }, { -12, 10, -23 }, 2).Merge(blueprint, meshes.NewBatch());

        scene.UseShaderFromFile(RES("shader.vert"), RES("shader.frag"));
        scene.SetProjection(Matrix3D::PerspectiveFov(90.0_deg, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        flatShader = Graphics::Shader::FromFile(RES("flat.vert"), RES("flat.frag"), RES("flat.geom"));
        normalShader = Graphics::Shader::FromFile(RES("norm.vert"), RES("norm.frag"), RES("norm.geom"));

        camera.position = { 0, 0, 0 };
        camera.yaw = 2.1558828f; camera.pitch = -0.891116f;
        camera.speed = 7.0f;
        camera.sensitivity = 0.12f;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5f;
        camera.smoothZoom = 120;
    }

    void TestGeometryShader::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestGeometryShader::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());
        const Math::Matrix3D norm = scene->camera.InvTransRot().Transpose();

        flatShader.Bind();
        flatShader.SetUniformInt("faceIndex", displayFace);

        scene.BeginContext();
        scene.AddMesh(meshes);
        scene.EndContext();
        scene.DrawContext(UseShaderWithArgs(useFlatShading ? flatShader : scene->shader, {
            { "ambientStrength", ambStrength },
        }));
        if (useGeomShader) {
            scene.DrawContext(UseShaderWithArgs(normalShader, {
                { "normalColor", normColor },
                { "normalMagnitude", normMag },
                { "normMat", norm },
            }));
        }
    }

    void TestGeometryShader::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditScalar("Display Face", displayFace, 0.015, iRange { -1, 20 });

        ImGui::Checkbox("Use Flat Shading", &useFlatShading);
        ImGui::EditScalar("Ambient", ambStrength, 0.01f, fRange { 0.0f, 1.0f });

        ImGui::Separator();
        ImGui::Checkbox("Enable Geometry Shader", &useGeomShader);
        if (useGeomShader) {
            ImGui::EditColor("Normal Line Color", normColor);
            ImGui::EditScalar("Normal Length", normMag, 0.1f);
        }

        ImGui::EditCameraController("Camera", camera);
    }

    void TestGeometryShader::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
        Graphics::Render::DisableCullFace();
    }
} // Test