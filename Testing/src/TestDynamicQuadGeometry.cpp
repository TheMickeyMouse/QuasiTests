#include "TestDynamicQuadGeometry.h"
#include "Quad.h"

#include "imgui.h"

namespace Test {
    Maths::colorf TestDynamicQuadGeometry::COLORS[8] = {
        Maths::colorf::BETTER_RED(),
        Maths::colorf::BETTER_GREEN(),
        Maths::colorf::BETTER_BLUE(),
        Maths::colorf::BETTER_MAGENTA(),
        Maths::colorf::BETTER_YELLOW(),
        Maths::colorf::BETTER_CYAN(),
        Maths::colorf::BETTER_ORANGE(),
        Maths::colorf::BETTER_PURPLE(),
    };
    
    void TestDynamicQuadGeometry::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<VertexColor3D>(8 * 4, 8 * 2);

        render.UseShader(Graphics::Shader::StdColored);
        render.SetProjection(projection);

        quads.push_back(NewQuad());
        quads.back().Bind(render);
    }

    void TestDynamicQuadGeometry::OnRender(Graphics::GraphicsDevice& gdevice) {
        render.ResetData();
        render.Render();
    }

    void TestDynamicQuadGeometry::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        uint quadCount = quads.size();

        auto& verts = quads.back().GetVertices();
        ImGui::DragFloat3("Quad Vertex 1", verts[0].Position.begin());
        ImGui::DragFloat3("Quad Vertex 2", verts[1].Position.begin());
        ImGui::DragFloat3("Quad Vertex 3", verts[2].Position.begin());
        ImGui::DragFloat3("Quad Vertex 4", verts[3].Position.begin());

        if (ImGui::Button("Add Quad") && !isMax) {
            if (quadCount >= MAX_QUAD) { isMax = true; } else {
                quads.push_back(NewQuad());
                quads.back().Bind(render);

                isMin = false;
            }
        }
        
        if (isMax) ImGui::Text("Too Many Quads! (Limit: %i)", MAX_QUAD); 
        
        if (ImGui::Button("Remove Quad") && !isMin) {
            if (quadCount <= 1) { isMin = true; } else {
                quads.back().Unbind();
                quads.pop_back();
                isMax = false;
            }
        }

        if (isMin) ImGui::Text("Can't Delete All Quads!");
    }

    void TestDynamicQuadGeometry::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }

    Graphics::Mesh<VertexColor3D> TestDynamicQuadGeometry::NewQuad() {
        Graphics::Primitives::Quad quad = { 0, { 50, 0, 0 }, { 0, 50, 0 } };
        return quad.IntoMesh<VertexColor3D>()
                   .ApplyMaterial(&VertexColor3D::Color, COLORS[quads.size()]);
    }
}
