#include "TestMaterialMaps.h"

#include "imgui.h"
#include "ModelLoading/OBJModelLoader.h"
#include "Meshes/CubeNormless.h"

namespace Test {
    void TestMaterialMaps::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Graphics::VertexTextureNormal3D>();
        lightScene = gdevice.CreateNewRender<Graphics::VertexColor3D>(8, 12);

        Graphics::OBJModelLoader mloader;
        mloader.LoadFile(res("boxes.obj"));

        meshes = mloader.GetModel().RetrieveMeshes();

        scene.BindMeshes(meshes);

        diffuseMap = Graphics::Texture::LoadPNG(res("diffuse.png"));
        specularMap = Graphics::Texture::LoadPNG(res("specular.png"));
        diffuseMap.Activate(0);
        specularMap.Activate(1);

        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));

        scene->shader.Bind();
        scene->shader.SetUniformTex("diffuseMap", diffuseMap);
        scene->shader.SetUniformTex("specularMap", specularMap);
        scene->shader.Unbind();

        using namespace Graphics::VertexBuilder;
        lightSource = Graphics::MeshUtils::CubeNormless(Graphics::VertexColor3D::Blueprint {
            .Position = GetPosition {},
            .Color = Constant { Math::fColor { 1 } }
        });
        lightScene.BindMesh(lightSource);
        lightScene.UseShader(Graphics::Shader::StdColored);

        camera.position = { 4.116068, 10.243308, 4.7378902 };
        camera.yaw = -10.371004f; camera.pitch = -0.8459675f;
        camera.speed = 1.5;
        camera.sensitivity = 0.12f;
        camera.fov = 45;
        camera.fovRange = { 1, 45 };
        camera.zoomRatio = 0.5;
        camera.smoothZoom = 120;
    }

    void TestMaterialMaps::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestMaterialMaps::OnRender(Graphics::GraphicsDevice& gdevice) {
        lightScene.SetProjection(camera.GetProjMat());
        lightScene.SetCamera(camera.GetViewMat());

        lightSource.ApplyMaterial(&Graphics::VertexColor3D::Color, lightColor.with_alpha(1));
        lightSource.SetTransform(Math::Matrix3D::translate_mat(lightPos));
        lightScene.ResetData();
        lightScene.Render();

        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());

        scene.ResetData();
        scene.Render({
            { "lightPosition",     lightPos },
            { "lightColor",        lightColor },
            { "ambientStrength",   ambientStrength },
            { "viewPosition",      camera.position },
            { "specularIntensity", specularStrength },
        });
    }

    void TestMaterialMaps::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Light Position", lightPos.begin());
        ImGui::ColorEdit3("Light Color", lightColor.begin());
        ImGui::DragFloat("Ambient Strength", &ambientStrength, 0.01f);
        ImGui::DragFloat("Specular Strength", &specularStrength, 0.01f);

        camera.ImGuiEdit();
    }

    void TestMaterialMaps::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
        lightScene.Destroy();
    }
}
