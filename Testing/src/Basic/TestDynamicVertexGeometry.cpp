#include "TestDynamicVertexGeometry.h"

#include "imgui.h"

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
        ImGui::DragFloat2("Red    Vertex [0]", vertices[0].Position.begin());
        ImGui::DragFloat2("Green  Vertex [1]", vertices[1].Position.begin());
        ImGui::DragFloat2("Blue   Vertex [2]", vertices[2].Position.begin());
        ImGui::DragFloat2("Gray   Vertex [3]", vertices[3].Position.begin());

        ImGui::DragFloat2("Purple Vertex [4]", vertices[4].Position.begin());
        ImGui::DragFloat2("Yellow Vertex [5]", vertices[5].Position.begin());
        ImGui::DragFloat2("Cyan   Vertex [6]", vertices[6].Position.begin());
        ImGui::DragFloat2("White  Vertex [7]", vertices[7].Position.begin());
    }

    void TestDynamicVertexGeometry::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }
}
