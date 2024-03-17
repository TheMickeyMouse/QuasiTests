#include "TestCubeMap.h"
#include "MeshUtils.h"

namespace Test {
    void TestCubeMap::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Vertex>();
        skybox = Graphics::MeshUtils::CubeMeshNorm(
            [](const Graphics::VertexNormal3D& v, uint) -> Vertex {
                return { v.Position, v.Position, v.Normal }; });

        cubemap = Graphics::Texture::LoadCubemapPNG(
            { res("right.jpg"), res("left.jpg"),
              res("top.jpg"),   res("bottom.jpg"),
              res("front.jpg"), res("back.jpg") });

        cubemap.Activate();

        scene.UseShaderFromFile(res("vertex.vert"), res("fragment.frag"));
        scene.GetShader().Bind();
        scene.GetShader().SetUniformTex("cubemap", cubemap);

        scene.SetProjection(Maths::mat3D::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f));

        camera.position = { 0.5531298, 0.82467157, -1.2348987 };
        camera.yaw = 2.2044506f; camera.pitch = 0.20220234f;
        camera.speed = 1.5;
        camera.sensitivity = 0.12f;
        camera.fov = 90;
        camera.fovRange = { 1, 90 };
        camera.zoomRatio = 0.5;
        camera.smoothZoom = 120;
    }

    void TestCubeMap::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        camera.Update(gdevice, deltaTime);
    }

    void TestCubeMap::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetProjection(camera.GetProjMat());
        scene.SetCamera(camera.GetViewMat());
        scene.ClearData();

        scene.AddNewMeshes(skybox);
        scene.Render();
    }

    void TestCubeMap::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        camera.ImGuiEdit();
    }

    void TestCubeMap::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
    }
}
