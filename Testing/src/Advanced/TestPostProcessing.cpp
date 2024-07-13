#include "TestPostProcessing.h"

#include "Constants.h"
#include "imgui.h"
#include "Mesh.h"
#include "Meshes/CubeNormless.h"
#include "Meshes/Plane.h"
#include "Meshes/Quad.h"

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
                .Color = Constant { Math::fColor::color_id(i) }
            }, Math::Transform3D::Scaling(s)));

            cubes[i].SetTransform(Math::Transform3D::Translation(Math::fVector3 { (Math::Corner3D)i }));
        }
        cubes.push_back(
        Graphics::MeshUtils::CubeNormless(Graphics::VertexColor3D::Blueprint {
            .Position = GetPosition {},
            .Color = Constant { Math::fColor::BETTER_GRAY() }
        }, Math::Transform3D::Scaling(s)));

        scene.UseShader(Graphics::Shader::StdColored);
        scene.SetProjection(Math::Matrix3D::perspective_fov(90.0f, gdevice.GetAspectRatio(), 0.01f, 100.0f));

        const auto [winX, winY] = gdevice.GetWindowSize();
        fbo.Create();
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

        screenQuad = Graphics::MeshUtils::Quad(Graphics::VertexTexture2D::Blueprint {
                        .Position = GetPosition {},
                        .TextureCoordinate = FromArg<PositionArg2D>([] (const Math::fVector2& v) { return (v + 1) * 0.5f; })
                    });

        const String vert = res("vertex.vert");
        postProcessingQuad.UseShaderFromFile(vert, res("none.frag"));
        // inv shader
        shaderInv = Graphics::Shader::FromFile(vert, res("invert.frag"));
        // hsl shader
        shaderHsv = Graphics::Shader::FromFile(vert, res("hsv.frag"));
        // blur shader
        shaderBlur = Graphics::Shader::FromFile(vert, res("simple_blur.frag"));
        // edge detect shader
        shaderEdgeDetect = Graphics::Shader::FromFile(vert, res("simple_ed.frag"));

        currShader = &postProcessingQuad->shader;
        renderResult.Activate(0);
    }

    void TestPostProcessing::OnUpdate(Graphics::GraphicsDevice& gdevice, float deltaTime) {
        modelRotation += turnSpeed * deltaTime;
        modelRotation = modelRotation % Math::TAU;
    }

    void TestPostProcessing::OnRender(Graphics::GraphicsDevice& gdevice) {
        const Math::Matrix3D mat = Math::Matrix3D::transform(modelTranslation, modelScale, modelRotation);

        scene.SetCamera(mat);

        Graphics::Render::EnableDepth();
        if (usePostProcessing) {
            fbo.Bind();
            Graphics::Render::Clear();
        }

        scene.Draw(cubes);

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

            // Graphics::Render::Draw(postProcessingQuad.GetRenderData(), *currShader);
            postProcessingQuad.Draw(screenQuad, UseShader(*currShader, false));
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
            TAB_ITEM(NONE, "None", postProcessingQuad->shader, )
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
