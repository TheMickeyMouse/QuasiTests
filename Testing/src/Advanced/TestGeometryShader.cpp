#include "TestGeometryShader.h"

#include <imgui.h>

#include "MeshUtils.h"
#include "Tri.h"

namespace Test {
    void TestGeometryShader::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(2048, 1024);

        using namespace Graphics::VertexBuilder;
        sphere = Graphics::MeshUtils::Sphere(20, Vertex::Blueprint {
            .Position = GetPosition {},
            .Normal = GetNormal {},
            .Color = Constant { Maths::colorf::BETTER_AQUA() }
        }, Maths::mat3D::scale_mat(10.0f));

        icosphere = Graphics::MeshUtils::Icosphere(2, Vertex::Blueprint {
            .Position = GetPosition {},
            .Normal = GetNormal {},
            .Color = Constant { Maths::colorf::BETTER_RED() }
        }, Maths::mat3D::scale_mat(10.0f));

        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        scene.SetProjection(Maths::mat3D::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f));

        flatShader = Graphics::Shader::FromFile(res("flat.vert"), res("flat.frag"), res("flat.geom"));
        normalShader = Graphics::Shader::FromFile(res("norm.vert"), res("norm.frag"), res("norm.geom"));

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
        const Maths::mat3D translate = Maths::mat3D::translate_mat({ 0, 0, 30 }),
                           norm = scene.GetView().inv().transpose();

        flatShader.Bind();
        flatShader.SetUniformInt("faceIndex", displayFace);

        scene.ClearData();
        scene.AddNewMeshes(sphere);
        scene.Render(useFlatShading ? flatShader : scene.GetShader(), {
            { "lightDirection", Maths::fvec3::from_spheric(1, lightYaw, lightPitch) },
            { "ambientStrength", ambStrength },
            { "u_model", Maths::mat3D::identity() }
        });
        if (useGeomShader) {
            scene.Render(normalShader, {
                { "normalColor", normColor },
                { "normalMagnitude", normMag },
                { "u_model", Maths::mat3D::identity() },
                { "normMat", norm },
            });
        }

        scene.ClearData();
        scene.AddNewMeshes(icosphere);
        scene.Render(useFlatShading ? flatShader : scene.GetShader(), {
            { "u_model", translate }
        });
        if (useGeomShader) {
            scene.Render(normalShader, {
                { "u_model", translate },
            });
        }
    }

    void TestGeometryShader::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::SliderInt("Display Face", &displayFace, -1, 20);

        ImGui::Checkbox("Use Flat Shading", &useFlatShading);
        ImGui::SliderFloat("Light Yaw", &lightYaw, -Maths::PI, Maths::PI);
        ImGui::SliderFloat("Light Pitch", &lightPitch, -Maths::HALF_PI * 0.95f, Maths::HALF_PI * 0.95f);
        ImGui::SliderFloat("Ambient", &ambStrength, 0.0f, 1.0f);

        ImGui::Separator();
        ImGui::Checkbox("Enable Geometry Shader", &useGeomShader);
        if (useGeomShader) {
            ImGui::ColorEdit4("Normal Line Color", normColor.begin());
            ImGui::DragFloat("Normal Length", &normMag, 0.1f);
        }

        camera.ImGuiEdit();
    }

    void TestGeometryShader::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }
} // Test