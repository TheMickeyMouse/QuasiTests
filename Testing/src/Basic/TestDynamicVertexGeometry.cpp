#include "TestDynamicVertexGeometry.h"

#include "imgui.h"
#include "Extension/ImGuiExt.h"

namespace Test {
    void TestDynamicVertexGeometry::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Graphics::VertexColor2D>(8, 4);

        render.UseShader(Graphics::Shader::StdColored);
        render.SetProjection(projection);

        using namespace Math;
        Vec<Graphics::VertexColor2D> vertices = {
            { { -240.0f, -80.0f }, fColor::RED()   },
            { { -80.00f, -80.0f }, fColor::GREEN() },
            { { -80.00f, +80.0f }, fColor::BLUE()  },
            { { -240.0f, +80.0f }, fColor::GRAY()  },
            
            { { +80.00f, -80.0f }, fColor::MAGENTA() },
            { { +240.0f, -80.0f }, fColor::YELLOW()  },
            { { +240.0f, +80.0f }, fColor::CYAN()    },
            { { +80.00f, +80.0f }, fColor::WHITE()   },
        };

        Vec<Graphics::TriIndices> indices = {
            { 0, 1, 2 },  { 2, 3, 0 },
            { 4, 5, 6 },  { 6, 7, 4 }
        };

        mesh = Graphics::Mesh(std::move(vertices), std::move(indices));
    }

    void TestDynamicVertexGeometry::OnRender(Graphics::GraphicsDevice& gdevice) {
        render.Draw(mesh);
    }

    void TestDynamicVertexGeometry::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        Graphics::VertexColor2D* vertices = mesh.vertices.data();
        ImGui::EditVector("Red    Vertex [0]", vertices[0].Position);
        ImGui::EditVector("Green  Vertex [1]", vertices[1].Position);
        ImGui::EditVector("Blue   Vertex [2]", vertices[2].Position);
        ImGui::EditVector("Gray   Vertex [3]", vertices[3].Position);

        ImGui::EditVector("Purple Vertex [4]", vertices[4].Position);
        ImGui::EditVector("Yellow Vertex [5]", vertices[5].Position);
        ImGui::EditVector("Cyan   Vertex [6]", vertices[6].Position);
        ImGui::EditVector("White  Vertex [7]", vertices[7].Position);
    }

    void TestDynamicVertexGeometry::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
