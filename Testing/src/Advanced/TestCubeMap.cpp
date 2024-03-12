#include "TestCubeMap.h"

namespace Test {
    void TestCubeMap::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        skybox = Graphics::MeshUtils::CubeMesh(0, 10, 10, 10)
                .Convert<Vertex>([](const VertexColor3D& v) -> Vertex { return { v.Position, 2 * v.Color.as_rgbf() - 1, 0 }; });
        scene.BindMeshes(skybox);

        cubemap = Graphics::Texture::LoadCubemapPNG(
            { res("right.png"), res("left.png"),
              res("up.png"),    res("down.png"),
              res("front.png"), res("back.png") });

        cubemap.Activate();

        scene.UseShaderFromFile(res("vertex.vert"), res("fragment.frag"));
        scene.GetShader().Bind();
        scene.GetShader().SetUniformTex("cubemap", cubemap);

        scene.SetProjection(Maths::mat3D::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f));
    }

    void TestCubeMap::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestCubeMap::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());
        scene.ResetData();
        scene.Render();
    }

    void TestCubeMap::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        camera.ImGuiEdit();
    }

    void TestCubeMap::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }
}
