#include "TestDynamicVertexGeometry.h"

#include "Utils/Extension/ImGuiExt.h"

namespace Test {
    void TestDynamicVertexGeometry::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<Vertex>(8, 4);

        render.UseShader(Graphics::Shader::StdColored);
        render.SetProjection(projection);

        using namespace Math;
        Vec<Vertex> vertices = Vec<Vertex>::New({
            { { -240.0f, -80.0f }, "red"_fColor     },
            { { -80.00f, -80.0f }, "green"_fColor   },
            { { -80.00f, +80.0f }, "blue"_fColor    },
            { { -240.0f, +80.0f }, "gray"_fColor    },
            { { +80.00f, -80.0f }, "magenta"_fColor },
            { { +240.0f, -80.0f }, "yellow"_fColor  },
            { { +240.0f, +80.0f }, "cyan"_fColor    },
            { { +80.00f, +80.0f }, "white"_fColor   },
        });

        Vec<Graphics::TriIndices> indices = Vec<Graphics::TriIndices>::New({
            { 0, 1, 2 },  { 2, 3, 0 },
            { 4, 5, 6 },  { 6, 7, 4 }
        });

        mesh = Graphics::Mesh(std::move(vertices), std::move(indices));
    }

    void TestDynamicVertexGeometry::OnRender(Graphics::GraphicsDevice& gdevice) {
        render.Draw(mesh);
    }

    void TestDynamicVertexGeometry::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        Vertex* vertices = mesh.vertices.Data();
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
