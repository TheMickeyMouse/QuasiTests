#include "TestMaterialMaps.h"

#include "imgui.h"
#include "Model Loading/OBJModelLoader.h"

namespace Test {
    void TestMaterialMaps::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        lightScene = gdevice.CreateNewRender<VertexColor3D>(8, 12);

        Graphics::OBJModelLoader mloader;
        mloader.LoadFile(res("boxes.obj"));

        auto m = mloader.GetModel().RetrieveMeshes();
        meshes.reserve(m.size());
        std::ranges::transform(m, std::back_inserter(meshes),
           [](const Graphics::Mesh<Graphics::OBJVertex>& m) -> Graphics::Mesh<Vertex> {
               return m.Convert<Vertex>(
                   [](const Graphics::OBJVertex& v) -> Vertex {
                       return { v.Position, v.TextureCoordinate, v.Normal };
                   }); });

        scene.BindMeshes(meshes);

        diffuseMap = Graphics::Texture(res("diffuse.png"), true);
        specularMap = Graphics::Texture(res("specular.png"), true);
        diffuseMap.Activate(0);
        specularMap.Activate(1);

        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));

        scene.GetShader().Bind();
        scene.GetShader().SetUniformTex("diffuseMap", diffuseMap);
        scene.GetShader().SetUniformTex("specularMap", specularMap);
        scene.GetShader().Unbind();

        lightSource = Graphics::MeshUtils::CubeMesh(0, 1, 1, 1);
        lightScene.BindMeshes(lightSource);
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

        lightSource.ApplyMaterial(&VertexColor3D::Color, lightColor.with_alpha(1));
        lightSource.SetTransform(Maths::mat3D::translate_mat(lightPos));
        lightScene.ResetData();
        lightScene.Render();

        scene.GetShader().Bind();
        scene.GetShader().SetUniform3F("lightPosition", lightPos.begin());
        scene.GetShader().SetUniform3F("lightColor", lightColor.begin());
        scene.GetShader().SetUniform1F("ambientStrength", ambientStrength);
        scene.GetShader().SetUniform3F("viewPosition", camera.position.begin());
        scene.GetShader().SetUniform1F("specularIntensity", specularStrength);

        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());

        scene.ResetData();
        scene.Render();
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
    }
}
