#include "TestLightCasters.h"

#include "imgui.h"
#include "GLs/VertexBlueprint.h"
#include "GUI/ImGuiExt.h"
#include "ModelLoading/OBJModelLoader.h"
#include "Meshes/Cube.h"

namespace Test {
    void TestLightCasters::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        lightScene = gdevice.CreateNewRender<Graphics::VertexColor3D>();

        Graphics::OBJModelLoader mloader;
        mloader.LoadFile(res("lights.obj").IntoCStr());
        Graphics::OBJModel model = mloader.RetrieveModel();

        materials = std::move(model.materials);
        meshes.Reserve(model.objects.Length());
        for (Graphics::OBJObject& obj : model.objects) {
            meshes.Push(
                std::move(obj.mesh).GeometryConvert(
                    [&] (const Graphics::OBJVertex& v) { return Vertex { v.Position, v.Normal, obj.materialIndex }; }
            ));
        }

        scene.UseShaderFromFile(res("shader.vert").IntoCStr(), res("shader.frag").IntoCStr());
        scene.SetProjection(Math::Matrix3D::PerspectiveFov(90.0_deg, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        camera.position = { 8.746245, 16.436476, 7.217131 };
        camera.yaw = -5.5221653; camera.pitch = 1.1316143;
        camera.speed = 5;
        camera.sensitivity = 0.12;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5;
        camera.smoothZoom = 120;

        lights.Reserve(MAX_LIGHTS);
        AddPointLight({
            .position = { -0, 7, 0 },
            .constant = 1,
            .linear = 0,
            .quadratic = 0
        }, { 1, 1, 1 });

        AddPointLight({
            .position = { 12, 3, -14 },
            .constant = 1,
            .linear = 0,
            .quadratic = 0.001f,
         }, { 1, 0, 0 });

        AddPointLight({
            .position = { -9, 3, 6 },
            .constant = 1,
            .linear = 0,
            .quadratic = 0.001f,
         }, { 0, 0.6176472f, 1 });

        AddPointLight({
            .position = { 12, 3, 16 },
            .constant = 1,
            .linear = 0,
            .quadratic = 0.001f,
         }, { 0.2647059f, 1, 0 });

        lightScene.UseShader(Graphics::Shader::StdColored);
    }

    void TestLightCasters::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestLightCasters::OnRender(Graphics::GraphicsDevice& gdevice) {
        lightScene.SetProjection(camera.GetProjMat());
        lightScene.SetCamera(camera.GetViewMat());
        lightScene.Draw(lightMeshes);

        scene->shader.Bind();
        for (u32 i = 0; i < materials.Length(); ++i) {
            UniformMaterial(Text::Format("materials[{}]", i), materials[i]);
        }

        for (u32 i = 0; i < lights.Length(); ++i) {
            UniformLight(Text::Format("lights[{}]", i), lights[i]);
        }

        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());
        scene.Draw(meshes, Graphics::UseArgs({
            { "ambientStrength",   ambientStrength },
            { "viewPosition",      camera.position },
            { "specularIntensity", specularStrength },
        }));
    }

    void TestLightCasters::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditScalar("Ambient Strength", ambientStrength, 0.01f);
        ImGui::EditScalar("Specular Strength", specularStrength, 0.01f);

        ImGui::EditCameraController("Camera", camera);

        if (ImGui::TreeNode("Lights")) {
            if (ImGui::Button("+") && lights.Length() < MAX_LIGHTS) {
                AddPointLight({
                    .position = 0,
                    .constant = 1, .linear = 0, .quadratic = 0
                }, 1);
            }
            ImGui::SameLine();
            if (ImGui::Button("-") && lights) {
                lights.Pop();
            }

            for (u32 i = 0; i < lights.Length(); ++i) {
                ImGui::EditLight(Text::Format("Light {}", i + 1), lights[i]);
                for (auto& v : lightMeshes[i].vertices)
                    v.Color = lights[i].color;
                lightMeshes[i].SetTransform(Math::Transform3D::Translate(lights[i].Position()));
            }
            ImGui::TreePop();
        }
    }

    void TestLightCasters::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
        lightScene.Destroy();
    }

    void TestLightCasters::UniformMaterial(const String& name, const Graphics::MTLMaterial& material) {
        Graphics::Shader& shader = scene->shader;
        shader.SetUniformArgs({
            { name + ".ambient",   material.Ka },
            { name + ".diffuse",   material.Kd },
            { name + ".specular",  material.Ks },
            { name + ".shininess", material.Ns },
        });
    }

    void TestLightCasters::UniformLight(const String& name, const Graphics::Light& light) {
        Graphics::Shader& shader = scene->shader;
        Math::fv3 top, bottom;
        light.Visit(
            [&] (const Graphics::SunLight& sun) { top = sun.direction; },
            [&] (const Graphics::PointLight& point) {
                top = point.position;
                bottom = { point.constant, point.linear, point.quadratic };
            },
            [&] (const Graphics::FlashLight& flash) {
                top = flash.position;
                bottom = { *flash.yaw, *flash.pitch, *flash.innerCut };
            }
        );
        shader.SetUniformArgs({
            { name + ".lightId", (int)light.GetTag() + 1 },
            { name + ".d1", top },
            { name + ".d2", bottom },
            { name + ".d3", light.Is<Graphics::FlashLight>() ? *light.As<Graphics::FlashLight>()->outerCut : 0 },
            { name + ".color", light.color },
        });
    }

    void TestLightCasters::AddPointLight(const Graphics::PointLight& point, const Math::fColor& color) {
        lights.Push({ Graphics::PointLight { point } });
        lights.Last().color = color;

        lightMeshes.Push(
            Graphics::Meshes::Cube().Create(QGLCreateBlueprint$(Graphics::VertexColor3D, (
                in (Position),
                out (Position) = Position;,
                out (Color) = color;
            )))
        );
        lightMeshes.Last().SetTransform(Math::Transform3D::Translate(point.position));
    }
}
