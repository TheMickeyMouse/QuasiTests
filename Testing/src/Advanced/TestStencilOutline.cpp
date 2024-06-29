#include "TestStencilOutline.h"

#include "imgui.h"
#include "Mesh.h"
#include "Meshes/CubeNormless.h"

namespace Test {
    void TestStencilOutline::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Graphics::VertexColor3D>();

        using namespace Graphics::VertexBuilder;
        constexpr float s = 0.3f;
        for (int i = 0; i < 8; ++i) {
            meshes.push_back(Graphics::MeshUtils::CubeNormless(Graphics::VertexColor3D::Blueprint {
                .Position = GetPosition {},
                .Color = Constant { Math::fColor::color_id(i) }
            }, Math::Matrix3D::scale_mat(s)));
            meshes[i].SetTransform(Math::Matrix3D::translate_mat(Math::fVector3 { (Math::Corner3D)i }));
        }
        meshes.push_back(Graphics::MeshUtils::CubeNormless(Graphics::VertexColor3D::Blueprint {
            .Position = GetPosition {},
            .Color = Constant { Math::fColor::BETTER_GRAY() }
        }, Math::Matrix3D::scale_mat(s)));

        outlinedMeshes = meshes; // yes we need a copy
        for (auto& o : outlinedMeshes) o.Scale(1.1f);

        scene.BindMeshes(meshes);
        scene.UseShader(Graphics::Shader::StdColored);
        scene.SetProjection(Math::Matrix3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        outlineShader = Graphics::Shader::FromFile(res("vertex.vert"), res("outline.frag"));

        Graphics::Render::EnableStencil();
        Graphics::Render::UseStencilTest(Graphics::CmpOperation::NOTEQUAL, 1); // always pass stencil test
        Graphics::Render::UseStencilWriteOp(
            Graphics::StencilOperation::KEEP,
            Graphics::StencilOperation::KEEP,
            Graphics::StencilOperation::REPLACE // set to 1 (ref from stencil func)
        );
        Graphics::Render::EnableStencilWrite(); // write to stencil
    }

    void TestStencilOutline::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        modelRotation += turnSpeed * deltaTime;
    }

    void TestStencilOutline::OnRender(Graphics::GraphicsDevice& gdevice) {
        const Math::Matrix3D mat = Math::Matrix3D::transform(modelTranslation, modelScale, modelRotation);
        scene.SetCamera(mat);
        scene.ResetData();
        scene.Render();

        Graphics::Render::UseStencilTest(Graphics::CmpOperation::NOTEQUAL, 1); // pass if it hasnt been set (drawn to) yet
        Graphics::Render::DisableDepth();
        Graphics::Render::DisableStencilWrite();

        scene.ClearData();
        scene.AddNewMeshes(outlinedMeshes);
        scene.Render(outlineShader, {{ "outlineColor", outlineColor }});

        Graphics::Render::UseStencilTest(Graphics::CmpOperation::ALWAYS, 1);
        Graphics::Render::EnableStencilWrite(); // write to stencil
        Graphics::Render::EnableDepth();
    }

    void TestStencilOutline::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Translation", modelTranslation.begin(), 0.01f);
        ImGui::DragFloat3("Scale"      , modelScale.begin(),       0.01f);
        ImGui::DragFloat3("Rotation"   , modelRotation.begin(),    0.01f);

        ImGui::DragFloat3("Spin Speed", turnSpeed.begin(), 0.01f, -10, 10);
        ImGui::ColorEdit4("Outline Color", outlineColor.begin());
    }

    void TestStencilOutline::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
        Graphics::Render::DisableStencil();
    }
}
