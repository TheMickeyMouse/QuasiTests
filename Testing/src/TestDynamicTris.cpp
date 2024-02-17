#include "TestDynamicTris.h"
#include "Tri.h"

#include "imgui.h"

namespace Test {
    Maths::colorf TestDynamicTris::COLORS[8] = {
        Maths::colorf::BETTER_RED(),
        Maths::colorf::BETTER_GREEN(),
        Maths::colorf::BETTER_BLUE(),
        Maths::colorf::BETTER_MAGENTA(),
        Maths::colorf::BETTER_YELLOW(),
        Maths::colorf::BETTER_CYAN(),
        Maths::colorf::BETTER_ORANGE(),
        Maths::colorf::BETTER_PURPLE(),
    };

    Maths::mat3D TestDynamicTris::ModelMatrix() {
        return Maths::mat3D::transform(modelTranslation, modelScale.with_z(1), { 0.0f, 0.0f, modelRotation });
    }

    void TestDynamicTris::OnInit(Graphics::GraphicsDevice& gdevice) {
        render = gdevice.CreateNewRender<VertexColor3D>(3 * 8, 8);

        render.UseShader(Graphics::Shader::StdColored);
        render.SetProjection(projection);
        
        tris.push_back(NewTri());
        tris.back().Bind(render);
    }

    void TestDynamicTris::OnRender(Graphics::GraphicsDevice& gdevice) {
        tris.back().SetTransform(ModelMatrix());
        
        render.ResetData();
        render.Render();
    }

    void TestDynamicTris::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat2("Current Tri Translation", modelTranslation.begin());
        ImGui::DragFloat2("Current Tri Scale      ", modelScale.begin(), 0.10f);
        ImGui::DragFloat ("Current Tri Rotation   ", &modelRotation, 0.05f);

        uint triCount = tris.size();

        if (ImGui::Button("Add Tri") && !isMax) {
            if (triCount >= MAX_TRIS) { isMax = true; } else {
                tris.push_back(NewTri());
                tris.back().Bind(render);
                // Graphics::Mesh(vc3, { { 0, 1, 2 } });
                modelTranslation = 0;
                modelScale = 1;
                modelRotation = 0.0f;
                
                isMin = false;
            }
        }
        
        if (isMax) ImGui::Text("Too Many Tris! (Limit: %i)", MAX_TRIS); 
        
        if (ImGui::Button("Remove Tri") && !isMin) {
            if (triCount <= 1) { isMin = true; } else {
                tris.back().Unbind();
                tris.pop_back();
                modelTranslation = 0;
                modelScale = 1;
                modelRotation = 0.0f;
                
                isMax = false;
            }
        }

        if (isMin) ImGui::Text("Can't Delete All Tris!");
    }

    void TestDynamicTris::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        render.Destroy();
    }

    Graphics::Mesh<VertexColor3D> TestDynamicTris::NewTri() {
        return Graphics::Primitives::Tri(
            { +00.0f, +50.0f, 0.0f },
            { -50.0f, -50.0f, 0.0f },
            { +50.0f, -50.0f, 0.0f }
        ).IntoMesh<VertexColor3D>()
         .ApplyMaterial(&VertexColor3D::Color, COLORS[tris.size()]);
    }
}
