#include "TestDynamicQuadGeometry.h"

#include "imgui.h"
#include "Meshes/Plane.h"

namespace Test {
    void TestDynamicQuadGeometry::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Graphics::VertexColor3D>(MAX_VERTEX, MAX_INDEX);

        render.UseShader(Graphics::Shader::StdColored);
        render.SetProjection(projection);

        quads.push_back(NewQuad());
        quads.back().Bind(render);
    }

    void TestDynamicQuadGeometry::OnRender(Graphics::GraphicsDevice& gdevice) {
        quads.back().SetTransform(Math::Matrix3D::transform(modelTranslation, modelScale.with_z(1), { 0.0f, 0.0f, modelRotation }));

        render.ResetData();
        render.Render();
    }

    void TestDynamicQuadGeometry::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        const usize quadCount = quads.size();

        auto& verts = quads.back().vertices;
        ImGui::DragFloat3("Quad Vertex 1", verts[0].Position.begin());
        ImGui::DragFloat3("Quad Vertex 2", verts[1].Position.begin());
        ImGui::DragFloat3("Quad Vertex 3", verts[2].Position.begin());
        ImGui::DragFloat3("Quad Vertex 4", verts[3].Position.begin());

        ImGui::DragFloat2("Current Tri Translation", modelTranslation.begin());
        ImGui::DragFloat2("Current Tri Scale      ", modelScale.begin(), 0.10f);
        ImGui::DragFloat ("Current Tri Rotation   ", &modelRotation, 0.05f);

        if (ImGui::Button("Add Quad") && !isMax) {
            if (quadCount >= MAX_QUAD) { isMax = true; } else {
                quads.push_back(NewQuad());
                quads.back().Bind(render);
                modelTranslation = 0;
                modelScale = 1;
                modelRotation = 0.0f;

                isMin = false;
            }
        }
        
        if (isMax) ImGui::Text("Too Many Quads! (Limit: %i)", MAX_QUAD); 
        
        if (ImGui::Button("Remove Quad") && !isMin) {
            if (quadCount <= 1) { isMin = true; } else {
                quads.back().Unbind();
                quads.pop_back();
                modelTranslation = 0;
                modelScale = 1;
                modelRotation = 0.0f;

                isMax = false;
            }
        }

        if (isMin) ImGui::Text("Can't Delete All Quads!");
    }

    void TestDynamicQuadGeometry::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }

    Graphics::Mesh<Graphics::VertexColor3D> TestDynamicQuadGeometry::NewQuad() {
        const Math::fColor color = Math::fColor::from_hsv((f32)quads.size() * 360.f / (f32)(MAX_QUAD + 1), 0.8f, 1.0f);
        return Graphics::MeshUtils::Plane([&] (const auto& m) {
            return Graphics::VertexColor3D { .Position = (m.Position % "xzy") * 80, .Color = color };
        });
    }
}
