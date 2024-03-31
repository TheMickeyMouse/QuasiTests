#include "TestGeometryShader.h"

#include "lambdas.h"
#include "Tri.h"

namespace Test {
    void TestGeometryShader::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Graphics::VertexColor3D>();

        meshes.AddTri({
            { -400.0f, -100.0f, 0.0f },
            { -400.0f, +100.0f, 0.0f },
            { -200.0f, -100.0f, 0.0f }
        }, LAMB(const Maths::fvec3& pos, (Graphics::VertexColor3D { pos, Maths::colorf::RED() })));

        meshes.AddTri({
            { +400.0f, -100.0f, 0.0f },
            { +400.0f, +100.0f, 0.0f },
            { +200.0f, -100.0f, 0.0f }
        }, LAMB(const Maths::fvec3& pos, (Graphics::VertexColor3D { pos, Maths::colorf::GREEN() })));

        scene.BindMeshes(meshes);
        scene.UseShaderFromFile(res("shader.vert"), res("shader.frag"), res("shader.geom"));
        scene.SetProjection(Maths::mat3D::ortho_projection({ -600.0f, 600.0f, -400.0f, 400.0f, -1.0f, 1.0f }));
    }

    void TestGeometryShader::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.ResetData();
        scene.Render();
    }

    void TestGeometryShader::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {

    }

    void TestGeometryShader::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }
} // Test