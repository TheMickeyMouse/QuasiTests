#include "TestCubeMap.h"

#include <imgui.h>

#include "GLs/VertexBlueprint.h"
#include "GUI/ImGuiExt.h"
#include "Meshes/Cube.h"

namespace Test {
    void TestCubeMap::OnInit(Graphics::GraphicsDevice& gdevice) {
        using Math::fv2;

        scene = gdevice.CreateNewRender<Vertex>();
        skybox = Graphics::MeshUtils::Cube(QGLCreateBlueprint$(Vertex, (
            in (Position, Normal),
            out (Position)          = Position;,
            out (TextureCoordinate) = Position;,
            out (Normal)            = Normal;
        )));

        cubemap = Graphics::Texture::LoadCubemapPNG(
            { res("right.jpg").IntoCStr(), res("left.jpg").IntoCStr(),
              res("top.jpg").IntoCStr(),   res("bottom.jpg").IntoCStr(),
              res("front.jpg").IntoCStr(), res("back.jpg").IntoCStr() });
        cubemap.Activate();

        boxTex = Graphics::Texture::LoadPNG(res("box.png").IntoCStr());
        boxTex.Activate();

        u32 i = 0;
        box = Graphics::MeshUtils::Cube(QGLCreateBlueprint$(Vertex, (
            in (Position, Normal),
            out (Position)          = Position;,
            out (TextureCoordinate) = (0.5f + fv2::FromCorner({ (bool)(i & 1), (bool)(i & 2) }) * 0.5f).AddZ(0);,
            out (Normal)            = Normal;,
            i++;
        )));

        cubemapShader = Graphics::Shader::FromFile(res("cubemap.vert").IntoCStr(), res("cubemap.frag").IntoCStr());
        boxShader     = Graphics::Shader::FromFile(res("box.vert")    .IntoCStr(), res("box.frag")    .IntoCStr());
        reflectShader = Graphics::Shader::FromFile(res("reflect.vert").IntoCStr(), res("reflect.frag").IntoCStr());
        refractShader = Graphics::Shader::FromFile(res("refract.vert").IntoCStr(), res("refract.frag").IntoCStr());

        scene.SetProjection(Math::Matrix3D::PerspectiveFov(90.0_deg, gdevice.GetAspectRatio(), 0.01f, 100.0f));

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

        scene.Draw(skybox, UseShaderWithArgs(cubemapShader, {{ "cubemap", cubemap }}));

        switch (shaderID) {
            case DIFFUSE_SHADER_ID:
                scene.Draw(box, UseShaderWithArgs(boxShader, {
                    { "boxTex", boxTex },
                    { "lightPosition", Math::fv3::FromSpheric(10.0f, lightYaw, lightPitch) },
                    { "ambientStrength", ambStrength }
                }));
            break;
            case REFLECTION_SHADER_ID:
                scene.Draw(box, UseShaderWithArgs(reflectShader, {
                    { "skybox", cubemap },
                    { "viewPosition", camera.position },
                    { "ambStrength", ambStrength }
                }));
            break;
            case REFRACTION_SHADER_ID:
                scene.Draw(box, UseShaderWithArgs(refractShader, {
                    { "skybox", cubemap },
                    { "viewPosition", camera.position },
                    { "ambStrength", ambStrength },
                    { "invRefractIndex", 1.0f / refractiveIndex }
                }));
            break;
            default:;
        }
    }

    void TestCubeMap::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditCameraController("Camera", camera);

        ImGui::EditYawPitch("Light Rotation", *lightYaw, *lightPitch);
        ImGui::EditScalar("Ambient Strength", ambStrength, 0.05f);

        ImGui::BeginTabBar("Shader Type");

        if (ImGui::TabItemButton("Diffuse"))    shaderID = DIFFUSE_SHADER_ID;
        if (ImGui::TabItemButton("Reflection")) shaderID = REFLECTION_SHADER_ID;
        if (ImGui::TabItemButton("Refraction")) shaderID = REFRACTION_SHADER_ID;

        switch (shaderID) {
            case REFRACTION_SHADER_ID:
                ImGui::EditScalar("Refractive Index", refractiveIndex, 0.05f);
            case DIFFUSE_SHADER_ID:
            case REFLECTION_SHADER_ID:
                ImGui::EditScalar("Ambient Strength", ambStrength, 0.05f);
            default:;
        }

        ImGui::EndTabBar();
    }

    void TestCubeMap::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }
}
