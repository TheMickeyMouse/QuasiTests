#include "TestAdvancedLighting.h"

#include "imgui.h"
#include "Model Loading/OBJModelLoader.h"


namespace Test {
    void TestAdvancedLighting::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(1024, 512);

        Graphics::OBJModelLoader mloader;
        mloader.LoadFile(res("untitled.obj"));
        Graphics::OBJModel model = mloader.RetrieveModel();

        // cube = std::move(model.objects[0].mesh);
        // light = std::move(model.objects[1].mesh);
        // scene.BindMeshes({ &light, &cube });
        materials = std::move(model.materials);
        meshes.reserve(model.objects.size());
        for (const Graphics::OBJObject& obj : model.objects) {
            meshes.emplace_back(
                obj.mesh.Convert<Vertex>(
                    [&](Graphics::OBJVertex v) {
                        return Vertex { v.Position, v.Normal, obj.materialIndex };
                    }));
        }
        scene.BindMeshes(meshes);

        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        scene.SetProjection(Maths::mat3D::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f));

        camera.trans = { 10, 5, 5 };
        camera.speed = 3;
    }

    void TestAdvancedLighting::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);

        // lightPos = Maths::fvec2::from_polar(5, (float)gdevice.GetIO().Time.currentTime).with_z(0);
        // light.SetTransform(Maths::mat3D::translate_mat(lightPos));
    }

    void TestAdvancedLighting::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.GetShader().Bind();
        scene.GetShader().SetUniform3F("lightPosition", lightPos.begin());
        scene.GetShader().SetUniform3F("lightColor", lightColor.begin());
        scene.GetShader().SetUniform1F("ambientStrength", ambientStrength);
        scene.GetShader().SetUniform3F("viewPosition", camera.trans.begin());
        scene.GetShader().SetUniform1F("specularIntensity", specularStrength);

        for (uint i = 0; i < materials.size(); ++i) {
            UniformMaterial(std::format("materials[{}]", i), materials[i]);
        }

        scene.SetCamera(camera.GetViewMat());
        scene.ResetData();
        scene.Render();
    }

    void TestAdvancedLighting::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        camera.ImGuiEdit();

        ImGui::DragFloat3("Light Position", lightPos.begin());
        ImGui::ColorEdit3("Light Color", lightColor.begin());
        ImGui::DragFloat("Ambient Strength", &ambientStrength, 0.01f);
        ImGui::DragFloat("Specular Strength", &specularStrength, 0.01f);
    }

    void TestAdvancedLighting::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestAdvancedLighting::UniformMaterial(const std::string& name, const Graphics::MTLMaterial& material) {
        Graphics::Shader& shader = scene.GetShader();
        shader.SetUniform3F(name + ".ambient", material.Ka.begin());
        shader.SetUniform3F(name + ".diffuse", material.Kd.begin());
        shader.SetUniform3F(name + ".specular", material.Ks.begin());
        shader.SetUniform1F(name + ".shininess", material.Ns);
    }
}
