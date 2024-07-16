#include "TestAdvancedLighting.h"

#include "Extension/ImGuiExt.h"
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
                obj.mesh.GeometryMap<Vertex>([&] (const Graphics::OBJVertex& v) {
                    return Vertex { v.Position, v.Normal, obj.materialIndex };
                }
            ));
        }

        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"));
        scene.SetProjection(Math::Matrix3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        camera.position = { 4.7875953, 9.015127, 0.9847422 };
        camera.yaw = -11.068636; camera.pitch = 1.0935568;
        camera.speed = 3;
        camera.sensitivity = 0.12;
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
        ImGui::EditVector("Light Position", lightPos);
        ImGui::EditColor ("Light Color", lightColor);
        ImGui::EditScalar("Ambient Strength", ambientStrength, 0.01f);
        ImGui::EditScalar("Specular Strength", specularStrength, 0.01f);

        ImGui::EditCameraController("Camera", camera);
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
