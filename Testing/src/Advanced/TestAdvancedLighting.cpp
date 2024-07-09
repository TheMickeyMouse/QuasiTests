#include "TestAdvancedLighting.h"

#include "imgui.h"
#include "ModelLoading/OBJModelLoader.h"

namespace Test {
    void TestAdvancedLighting::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>(1024, 512);

        Graphics::OBJModelLoader mloader;
        mloader.LoadFile(res("untitled.obj"));
        Graphics::OBJModel model = mloader.RetrieveModel();

        using namespace Graphics::VertexBuilder;
        materials = std::move(model.materials);
        meshes.reserve(model.objects.size());
        for (const Graphics::OBJObject& obj : model.objects) {
            meshes.emplace_back(
                obj.mesh.Convert<Vertex>(
                [&] (const Graphics::OBJVertex& v) { return Vertex { v.Position, v.Normal, obj.materialIndex }; }
            ));
        }

        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        scene.SetProjection(Math::Matrix3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        camera.position = { 3.3716135, 9.015127, 0.29202303 };
        camera.yaw = -3.4856012f; camera.pitch = -1.166767f;
        camera.speed = 3;
        camera.sensitivity = 0.12f;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5;
        camera.smoothZoom = 120;

        camera.Toggle(gdevice);
    }

    void TestAdvancedLighting::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);

        // lightPos = Math::fVector2::from_polar(5, (float)gdevice.GetIO().Time.currentTime).with_z(0);
        // light.SetTransform(Math::Matrix3D::translate_mat(lightPos));
    }

    void TestAdvancedLighting::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene->shader.Bind();

        for (uint i = 0; i < materials.size(); ++i) {
            UniformMaterial(std::format("materials[{}]", i), materials[i]);
        }

        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());
        scene.Draw(meshes, Graphics::UseArgs({
            { "lightPosition",     lightPos },
            { "lightColor",        lightColor },
            { "ambientStrength",   ambientStrength },
            { "viewPosition",      camera.position },
            { "specularIntensity", specularStrength }
        }));
    }

    void TestAdvancedLighting::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Light Position", lightPos.begin());
        ImGui::ColorEdit3("Light Color", lightColor.begin());
        ImGui::DragFloat("Ambient Strength", &ambientStrength, 0.01f);
        ImGui::DragFloat("Specular Strength", &specularStrength, 0.01f);

        camera.ImGuiEdit();
    }

    void TestAdvancedLighting::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }

    void TestAdvancedLighting::UniformMaterial(Str name, const Graphics::MTLMaterial& material) {
        scene->shader.SetUniformArgs({
            { std::format("{}.ambient",   name), material.Ka },
            { std::format("{}.diffuse",   name), material.Kd },
            { std::format("{}.specular",  name), material.Ks },
            { std::format("{}.shininess", name), material.Ns },
        });
    }
}
