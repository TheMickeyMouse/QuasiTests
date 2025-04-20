#include "TestGeometryShader.h"

#include <imgui.h>

#include "GLs/VertexBlueprint.h"
#include "Utils/Extension/ImGuiExt.h"
#include "Utils/Meshes/Icosphere.h"
#include "Utils/Meshes/Sphere.h"

namespace Test {
    void TestGeometryShader::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(2048, 1024);

        sphere = Graphics::MeshUtils::Sphere({ 20 }, QGLCreateBlueprint$(Vertex, (
            in (Position, Normal),
            out (Position) = Position;,
            out (Color)    = Math::fColor::BETTER_AQUA();,
            out (Normal)   = Normal;,
        )), Math::Transform3D::Scaling(10.0f));

        icosphere = Graphics::MeshUtils::Icosphere({ 2 }, QGLCreateBlueprint$(Vertex, (
            in (Position, Normal),
            out (Position) = Position;,
            out (Color)    = Math::fColor::BETTER_RED();,
            out (Normal)   = Normal;,
        )), Math::Transform3D::Scaling(10.0f));

        scene.UseShaderFromFile(res("shader.vert").IntoCStr(), res("shader.frag").IntoCStr());
        scene.SetProjection(Math::Matrix3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        flatShader = Graphics::Shader::FromFile(res("flat.vert").IntoCStr(), res("flat.frag").IntoCStr(), res("flat.geom").IntoCStr());
        normalShader = Graphics::Shader::FromFile(res("norm.vert").IntoCStr(), res("norm.frag").IntoCStr(), res("norm.geom").IntoCStr());

        camera.position = { 10.222575, 13.1479025, 10.771797 };
        camera.yaw = 2.1558828f; camera.pitch = -0.891116f;
        camera.speed = 7.0f;
        camera.sensitivity = 0.12f;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5f;
        camera.smoothZoom = 120;

        lightYaw = 2.62f;
        lightPitch = 0.5f;
    }

    void TestGeometryShader::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestGeometryShader::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());
        const Math::Matrix3D translate = Math::Matrix3D::translate_mat({ 0, 0, 30 }),
                           norm = scene->camera.inv().transpose();

        flatShader.Bind();
        flatShader.SetUniformInt("faceIndex", displayFace);

        scene.BeginContext();
        scene.AddMesh(sphere);
        scene.EndContext();
        scene.DrawContext(UseShaderWithArgs(useFlatShading ? flatShader : scene->shader, {
            { "lightDirection",  Math::fVector3::from_spheric(1, lightYaw, lightPitch) },
            { "ambientStrength", ambStrength },
            { "u_model",         Math::Matrix3D::identity() }
        }));
        if (useGeomShader) {
            scene.DrawContext(UseShaderWithArgs( normalShader, {
                { "normalColor", normColor },
                { "normalMagnitude", normMag },
                { "u_model", Math::Matrix3D::identity() },
                { "normMat", norm },
            }));
        }

        scene.BeginContext();
        scene.AddMesh(icosphere);
        scene.EndContext();
        scene.DrawContext(UseShaderWithArgs(useFlatShading ? flatShader : scene->shader, {
            { "u_model", translate }
        }));
        if (useGeomShader) {
            scene.DrawContext(UseShaderWithArgs(normalShader, {
                { "u_model", translate },
            }));
        }
    }

    void TestGeometryShader::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditScalar("Display Face", displayFace, 0.015, Math::iRange { -1, 20 });

        ImGui::Checkbox("Use Flat Shading", &useFlatShading);
        ImGui::EditRotation("Light Rotation", lightYaw, lightPitch);
        ImGui::EditScalar("Ambient", ambStrength, 0.01f, Math::fRange { 0.0f, 1.0f });

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
    }
} // Test