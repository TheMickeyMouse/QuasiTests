#include "TestCubeMap.h"

#include <imgui_stdlib.h>

#include "lambdas.h"
#include "MeshUtils.h"
#include "VertexConverter.h"

namespace Test {
    void TestCubeMap::OnInit(Graphics::GraphicsDevice& gdevice) {
        using namespace Graphics::VertexBuilder;
        using Maths::fvec2, Maths::Direction2D;

        scene = gdevice.CreateNewRender<Vertex>();
        skybox = Graphics::MeshUtils::Cube(Vertex::Blueprint {
            .Position = GetPosition {},
            .TextureCoord = GetPosition {},
            .Normal = GetNormal {}
        });

        cubemap = Graphics::Texture::LoadCubemapPNG(
            { res("right.jpg"), res("left.jpg"),
              res("top.jpg"),   res("bottom.jpg"),
              res("front.jpg"), res("back.jpg") });
        cubemap.Activate();

        boxTex = Graphics::Texture::LoadPNG(res("box.png"));
        boxTex.Activate();

        box = Graphics::MeshUtils::Cube(
            Vertex::Blueprint {
                .Position = GetPosition {},
                .TextureCoord = FromArg<VIndexArg>(λ(uint i, 0.5f + fvec2 { (Maths::Corner2D)(i % 4) } * 0.5f)),
                .Normal = GetNormal {}
            }
        );

        cubemapShader = Graphics::Shader::FromFile(res("cubemap.vert"), res("cubemap.frag"));

        boxShader     = Graphics::Shader::FromFile(res("box.vert"),     res("box.frag"));
        reflectShader = Graphics::Shader::FromFile(res("reflect.vert"), res("reflect.frag"));
        refractShader = Graphics::Shader::FromFile(res("refract.vert"), res("refract.frag"));

        scene.SetProjection(Maths::mat3D::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f));

        camera.position = { 0.5531298, 0.82467157, -1.2348987 };
        camera.yaw = 2.2044506f; camera.pitch = 0.20220234f;
        camera.speed = 1.5;
        camera.sensitivity = 0.12f;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5;
        camera.smoothZoom = 120;
    }

    void TestCubeMap::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestCubeMap::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());

        scene.ClearData();
        scene.AddNewMeshes(skybox);
        scene.Render(cubemapShader, {{ "cubemap", cubemap }});

        scene.ClearData();
        scene.AddNewMeshes(box);

        switch (shaderID) {
            case DIFFUSE_SHADER_ID:
                scene.Render(boxShader, {
                    { "boxTex", boxTex },
                    { "lightPosition", Maths::fvec3::from_polar(10.0f, lightYaw, lightPitch) },
                    { "ambientStrength", ambStrength }
                });
            break;
            case REFLECTION_SHADER_ID:
                scene.Render(reflectShader, {
                    { "skybox", cubemap },
                    { "viewPosition", camera.position },
                    { "ambStrength", ambStrength }
                });
            break;
            case REFRACTION_SHADER_ID:
                scene.Render(refractShader, {
                    { "skybox", cubemap },
                    { "viewPosition", camera.position },
                    { "ambStrength", ambStrength },
                    { "invRefractIndex", 1.0f / refractiveIndex }
                });
            break;
            default:;
        }
    }

    void TestCubeMap::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        camera.ImGuiEdit();

        ImGui::SliderFloat("Light Yaw",   &lightYaw,   -Maths::PI,             Maths::PI);
        ImGui::SliderFloat("Light Pitch", &lightPitch, -Maths::HALF_PI * 0.9f, Maths::HALF_PI * 0.9f);
        ImGui::DragFloat("Ambient Strength", &ambStrength, 0.05f);

        ImGui::BeginTabBar("Shader Type");

        if (ImGui::TabItemButton("Diffuse"))    shaderID = DIFFUSE_SHADER_ID;
        if (ImGui::TabItemButton("Reflection")) shaderID = REFLECTION_SHADER_ID;
        if (ImGui::TabItemButton("Refraction")) shaderID = REFRACTION_SHADER_ID;

        switch (shaderID) {
            case REFRACTION_SHADER_ID:
                ImGui::DragFloat("Refractive Index", &refractiveIndex, 0.05f);
            case DIFFUSE_SHADER_ID:
            case REFLECTION_SHADER_ID:
                ImGui::DragFloat("Ambient Strength", &ambStrength, 0.05f);
            default:;
        }

        ImGui::EndTabBar();
    }

    void TestCubeMap::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }
}
