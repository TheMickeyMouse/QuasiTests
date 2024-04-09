#include "TestPostProcessing.h"

#include "Constants.h"
#include "imgui.h"
#include "lambdas.h"
#include "Mesh.h"
#include "Quad.h"
#include "Meshes/CubeNormless.h"

namespace Test {
    void TestPostProcessing::OnInit(Graphics::GraphicsDevice& gdevice) {
        scene = gdevice.CreateNewRender<Graphics::VertexColor3D>(72, 108);
        postProcessingQuad = gdevice.CreateNewRender<Graphics::VertexTexture2D>(4, 2);

        cubes.reserve(9);

        using namespace Graphics::VertexBuilder;
        constexpr float s = 0.3f;
        for (int i = 0; i < 8; ++i) {
            cubes.push_back(Graphics::MeshUtils::CubeNormless(Graphics::VertexColor3D::Blueprint {
                .Position = GetPosition {},
                .Color = Constant { Maths::colorf::color_id(i) }
            }, Maths::mat4x4::scale_mat(s)));

            cubes[i].SetTransform(Maths::mat3D::translate_mat(Maths::fvec3 { (Maths::Corner3D)i }));
        }
        cubes.push_back(
        Graphics::MeshUtils::CubeNormless(Graphics::VertexColor3D::Blueprint {
            .Position = GetPosition {},
            .Color = Constant { Maths::colorf::BETTER_GRAY() }
        }, Maths::mat4x4::scale_mat(s)));

        scene.BindMeshes(cubes);
        scene.UseShader(Graphics::Shader::StdColored);
        scene.SetProjection(Maths::mat3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        const auto [winX, winY] = gdevice.GetWindowSize();
        fbo = Graphics::FrameBuffer {{}};
        fbo.Bind();

        renderResult = Graphics::Texture {
            nullptr, { winX, winY },
            { .load = { .format = Graphics::TextureFormat::RGB, .internalformat = Graphics::TextureIFormat::RGB_8 } }
        };

        fbo.Attach(renderResult);

        depthStencilAttachment = Graphics::RenderBuffer {
            Graphics::TextureIFormat::DEPTH_24_STENCIL_8, gdevice.GetWindowSize()
        };

        fbo.Bind();
        fbo.Attach(depthStencilAttachment, Graphics::AttachmentType::DEPTH_STENCIL);
        fbo.Complete();
        fbo.Unbind();

        screenQuad = Graphics::Primitives::Quad { 0, Maths::fvec3::RIGHT(), Maths::fvec3::UP() }
                    .IntoMesh<Graphics::Vertex3D>()
                    .Convert<Graphics::VertexTexture2D>(Graphics::VertexTexture2D::Blueprint {
                        .Position = CastPosition<Maths::fvec2> {},
                        .TextureCoordinate = FromArg<&Graphics::Vertex3D::Position>(LAMB(const auto& v, (v + 1) * 0.5f))
                    });
        postProcessingQuad.BindMeshes(screenQuad);

        const std::string vert = res("vertex.vert");
        postProcessingQuad.UseShaderFromFile(vert, res("none.frag"));
        // inv shader
        shaderInv = Graphics::Shader::FromFile(vert, res("invert.frag"));
        // hsl shader
        shaderHsv = Graphics::Shader::FromFile(vert, res("hsv.frag"));
        // blur shader
        shaderBlur = Graphics::Shader::FromFile(vert, res("simple_blur.frag"));
        // edge detect shader
        shaderEdgeDetect = Graphics::Shader::FromFile(vert, res("simple_ed.frag"));

        currShader = &postProcessingQuad.Shader();
        renderResult.Activate(0);
    }

    void TestPostProcessing::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        modelRotation += turnSpeed * deltaTime;
        modelRotation = modelRotation % Maths::TAU;
    }

    void TestPostProcessing::OnRender(Graphics::GraphicsDevice& gdevice) {
        const Maths::mat3D mat = Maths::mat3D::transform(modelTranslation, modelScale, modelRotation);

        scene.SetCamera(mat);

        Graphics::Render::EnableDepth();
        if (usePostProcessing) {
            fbo.Bind();
            Graphics::Render::Clear();
        }

        scene.ResetData();
        scene.Render();

        if (usePostProcessing) {
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

            postProcessingQuad.ResetData();
            // Graphics::Render::Draw(postProcessingQuad.GetRenderData(), *currShader);
            postProcessingQuad.Render(*currShader, {}, false);
        }
    }

    void TestPostProcessing::OnImGuiRender(Graphics::GraphicsDevice& gdevice) {
        ImGui::DragFloat3("Translation" , modelTranslation.begin(), 0.01f);
        ImGui::DragFloat3("Scale"       , modelScale.begin(),       0.01f);
        ImGui::DragFloat3("Rotation"    , modelRotation.begin(),    0.01f);

        ImGui::DragFloat3("Spin Speed", turnSpeed.begin(), 0.01f, -10, 10);

        ImGui::Separator();

        ImGui::Checkbox("Use Post Processing", &usePostProcessing);

#define TAB_ITEM(X, N, P, C) if (ImGui::BeginTabItem(N)) { currShader = &(P); ImGui::EndTabItem(); C }
        if (ImGui::BeginTabBar("Post Processing Shader")) {
            TAB_ITEM(NONE, "None", postProcessingQuad.Shader(), )
            TAB_ITEM(COLOR_INVERT, "Color Invert", shaderInv, )
            TAB_ITEM(COLOR_HSL, "Color Hue", shaderHsv,
                ImGui::DragFloat("Hue Shift", &hueShift, 0.01f, 0, 1);
                ImGui::DragFloat("Saturation Multiplier", &satMul, 0.01f, 0);
                ImGui::DragFloat("Value Shift", &valShift, 0.01f, -1, 1);)
            TAB_ITEM(BLUR, "Blur", shaderBlur, ImGui::DragFloat2("Blur Offset", effectOff.begin(), 0.1f); )
            TAB_ITEM(EDGE_DETECT, "Edge Detection", shaderEdgeDetect, ImGui::DragFloat2("Detect Offset", effectOff.begin(), 0.1f); )
            ImGui::EndTabBar();
        }
#undef TAB_ITEM
    }

    void TestPostProcessing::OnDestroy(Graphics::GraphicsDevice& gdevice) {
        scene.Destroy();
        Graphics::Render::EnableDepth();
    }
}
