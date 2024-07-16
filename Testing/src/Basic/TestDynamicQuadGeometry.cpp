#include "TestDynamicQuadGeometry.h"

#include "imgui.h"
#include "Extension/ImGuiExt.h"
#include "Meshes/Quad.h"

namespace Test {
    void TestDynamicQuadGeometry::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Graphics::VertexColor2D>(MAX_VERTEX, MAX_INDEX);

        render.UseShader(Graphics::Shader::StdColored);
        render.SetProjection(projection);

        quads.push_back(NewQuad());
    }

    void TestDynamicQuadGeometry::OnRender(Graphics::GraphicsDevice& gdevice) {
        render.Draw(quads);
    }

    void TestDynamicQuadGeometry::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        const usize quadCount = quads.size();

        auto& quad = quads.back();
        auto& verts = quad.vertices;
        ImGui::EditVector("Quad Vertex 1", verts[0].Position);
        ImGui::EditVector("Quad Vertex 2", verts[1].Position);
        ImGui::EditVector("Quad Vertex 3", verts[2].Position);
        ImGui::EditVector("Quad Vertex 4", verts[3].Position);

        ImGui::EditTransform("Transform", quad.modelTransform, 0.1f);

        if (ImGui::Button("Add Quad") && !isMax) {
            if (quadCount >= MAX_QUAD) { isMax = true; } else {
                quads.push_back(NewQuad());

                isMin = false;
            }
        }
        
        if (isMax) ImGui::Text("Too Many Quads! (Limit: %i)", MAX_QUAD); 
        
        if (ImGui::Button("Remove Quad") && !isMin) {
            if (quadCount <= 1) { isMin = true; } else {
                quads.pop_back();

                isMax = false;
            }
        }

        if (isMin) ImGui::Text("Can't Delete All Quads!");
    }

    void TestDynamicQuadGeometry::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }

    Graphics::Mesh<Graphics::VertexColor2D> TestDynamicQuadGeometry::NewQuad() {
        const Math::fColor color = Math::fColor::from_hsv((f32)quads.size() * 360.f / (f32)(MAX_QUAD + 1), 0.8f, 1.0f);
        return Graphics::MeshUtils::Quad([&] (const auto& m) {
            return Graphics::VertexColor2D { .Position = m.Position * 80, .Color = color };
        });
    }
}
