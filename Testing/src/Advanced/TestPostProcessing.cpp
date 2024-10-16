#include "TestPostProcessing.h"

#include "Mesh.h"
#include "VertexBlueprint.h"
#include "Extension/ImGuiExt.h"
#include "Meshes/CubeNormless.h"
#include "Meshes/Plane.h"
#include "Meshes/Quad.h"

namespace Test {
    void TestPostProcessing::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Graphics::VertexColor3D>(72, 108);
        postProcessingQuad = gdevice.CreateNewRender<Graphics::VertexTexture2D>(4, 2);

        cubes.reserve(9);

        constexpr float s = 0.3f;
        for (int i = 0; i < 8; ++i) {
            cubes.push_back(Graphics::MeshUtils::CubeNormless(QGLCreateBlueprint$(Graphics::VertexColor3D, (
                in (Position),
                out (Position) = Position;,
                out (Color) = Math::fColor::color_id(i);
            )), Math::Transform3D::Scaling(s)));

            cubes[i].SetTransform(Math::Transform3D::Translation(Math::fVector3::from_corner(i, 1)));
        }
        cubes.push_back(
        Graphics::MeshUtils::CubeNormless(QGLCreateBlueprint$(Graphics::VertexColor3D, (
                in (Position),
                out (Position) = Position;,
                out (Color) = Math::fColor::BETTER_GRAY();
        )), Math::Transform3D::Scaling(s)));

        scene.UseShader(Graphics::Shader::StdColored);
        scene.SetProjection(Math::Matrix3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        const auto [winX, winY] = gdevice.GetWindowSize();
        fbo = Graphics::FrameBuffer::New();
        fbo.Bind();

        renderResult = Graphics::Texture::New(
            nullptr, { winX, winY },
            { .load = { .format = Graphics::TextureFormat::RGB, .internalformat = Graphics::TextureIFormat::RGB_8 } }
        );

        fbo.Attach(renderResult);

        depthStencilAttachment = Graphics::RenderBuffer::New(
            Graphics::TextureIFormat::DEPTH_24_STENCIL_8, gdevice.GetWindowSize()
        );

        fbo.Bind();
        fbo.Attach(depthStencilAttachment, Graphics::AttachmentType::DEPTH_STENCIL);
        fbo.Complete();
        fbo.Unbind();

        screenQuad = Graphics::MeshUtils::Quad(QGLCreateBlueprint$(Graphics::VertexTexture2D, (
            in (Position),
            out (Position) = Position;,
            out (TextureCoordinate) = (Position + 1) * 0.5f;
        )));

        const String vert = res("vertex.vert");
        postProcessingQuad.UseShaderFromFile(vert, res("none.frag"));

        shaderInv = Graphics::Shader::FromFile(vert, res("invert.frag"));
        shaderHsv = Graphics::Shader::FromFile(vert, res("hsv.frag"));
        shaderBlur = Graphics::Shader::FromFile(vert, res("simple_blur.frag"));
        shaderEdgeDetect = Graphics::Shader::FromFile(vert, res("simple_ed.frag"));
        shaderOutline = Graphics::Shader::FromFile(res("outline.vert"), res("outline.frag"));

        currShader = &postProcessingQuad->shader;
        renderResult.Activate(0);
    }

    void TestPostProcessing::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        transform.rotation.rotate_by(turnSpeed.pow(deltaTime));
    }

    void TestPostProcessing::OnRender(Graphics::GraphicsDevice& gdevice) {
        scene.SetCamera(transform.TransformMatrix());

        if (currShader != &shaderOutline) {
            Graphics::Render::EnableDepth();
            if (usePostProcessing) {
                fbo.Bind();
                Graphics::Render::Clear();
            }
        }

        scene.Draw(cubes);

        if (usePostProcessing) {
            if (currShader == &shaderOutline) {
                Graphics::Render::UseStencilTest(Graphics::CmpOperation::NOTEQUAL, 1); // pass if it hasnt been set (drawn to) yet
                Graphics::Render::DisableDepth();
                Graphics::Render::DisableStencilWrite();

                for (auto& cube : cubes) cube.modelTransform.scale = outlineSize;
                scene.Draw(cubes, UseShaderWithArgs(shaderOutline, {{ "outlineColor", Math::fColor { 1 } }}));
                for (auto& cube : cubes) cube.modelTransform.scale = 1;

                Graphics::Render::UseStencilTest(Graphics::CmpOperation::ALWAYS, 1);
                Graphics::Render::EnableStencilWrite(); // write to stencil
                Graphics::Render::EnableDepth();
                return;
            }

            Graphics::Render::DisableDepth();
            fbo.Unbind();

            // Graphics::Render::SetClearColor(1);
            Graphics::Render::ClearColorBit();

            currShader->Bind();
            currShader->SetUniformTex("screenTexture", renderResult);
            if (currShader == &shaderBlur) {
                currShader->SetUniformFvec2("blurOff", effectOff);
            } else if (currShader == &shaderEdgeDetect) {
                currShader->SetUniformFvec2("detectOff", effectOff);
            } else if (currShader == &shaderHsv) {
                currShader->SetUniformFloat("dh", hueShift);
                currShader->SetUniformFloat("ds", satMul);
                currShader->SetUniformFloat("dv", valShift);
            }

            // Graphics::Render::Draw(postProcessingQuad.GetRenderData(), *currShader);
            postProcessingQuad.Draw(screenQuad, UseShader(*currShader, false));
        }
    }

    void TestPostProcessing::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::EditTransform("Transform", transform, 0.01);

        ImGui::EditQuatRotation("Spin Speed", turnSpeed);

        ImGui::Separator();

        ImGui::Checkbox("Use Post Processing", &usePostProcessing);

        Graphics::Shader* prev = currShader;
#define TAB_ITEM(X, N, P, C) if (ImGui::BeginTabItem(N)) { currShader = &(P); ImGui::EndTabItem(); C }
        if (ImGui::BeginTabBar("Post Processing Shader")) {
            TAB_ITEM(NONE, "None", postProcessingQuad->shader, )
            TAB_ITEM(COLOR_INVERT, "Color Invert", shaderInv, )
            TAB_ITEM(COLOR_HSL, "Color Hue", shaderHsv,
                ImGui::EditScalar("Hue Shift", hueShift, 0.01f, Math::fRange { 0, 1 });
                ImGui::EditScalar("Saturation Multiplier", satMul, 0.01f, Math::fRange { 0, 10 });
                ImGui::EditScalar("Value Shift", valShift, 0.01f, Math::fRange { -1, 1 });)
            TAB_ITEM(BLUR, "Blur", shaderBlur, ImGui::EditVector("Blur Offset", effectOff, 0.1f); )
            TAB_ITEM(EDGE_DETECT, "Edge Detection", shaderEdgeDetect, ImGui::EditVector("Detect Offset", effectOff, 0.1f); )
            TAB_ITEM(OUTLINE, "Outline (Stencil)", shaderOutline,
                ImGui::EditScalar("Outline Size", outlineSize, 0.01f, Math::fRange { 1, 2 });
            )
            ImGui::EndTabBar();
        }
#undef TAB_ITEM

        if (prev != currShader) {
            if (currShader == &shaderOutline) {
                Graphics::Render::EnableStencil();
                Graphics::Render::UseStencilTest(Graphics::CmpOperation::NOTEQUAL, 1); // always pass stencil test
                Graphics::Render::UseStencilWriteOp(
                    Graphics::StencilOperation::KEEP,
                    Graphics::StencilOperation::KEEP,
                    Graphics::StencilOperation::REPLACE // set to 1 (ref from stencil func)
                );
                Graphics::Render::EnableStencilWrite(); // write to stencil
            } else {
                Graphics::Render::DisableStencil();
            }
        }
    }

    void TestPostProcessing::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
        Graphics::Render::EnableDepth();
        Graphics::Render::DisableStencil();
    }
}
