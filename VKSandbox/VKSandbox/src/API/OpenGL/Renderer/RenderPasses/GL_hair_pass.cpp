#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "API/OpenGL/GL_backend.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

#include "Core/Audio.h"
#include "Input/Input.h"

namespace OpenGLRenderer {

    void UpdateHairDebugInput() {
        const RendererSettings& renderSettings = Config::GetRendererSettings();
        int peelCount = renderSettings.depthPeelCount;
        if (Input::KeyPressed(HELL_KEY_8) && peelCount < 7) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            Config::SetDepthPeelCount(peelCount + 1);
            std::cout << "Depth peel layer count: " << peelCount << "\n";
        }
        if (Input::KeyPressed(HELL_KEY_9) && peelCount > 0) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            Config::SetDepthPeelCount(peelCount - 1);
            std::cout << "Depth peel layer count: " << peelCount << "\n";
        }
    }

    void HairPass() {
        UpdateHairDebugInput();

        OpenGLShader* shader = GetShader("HairFinalComposite");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* hairFrameBuffer = GetFrameBuffer("Hair");

        if (!shader) return;
        if (!gBuffer) return;
        if (!hairFrameBuffer) return;

        // Setup state
        hairFrameBuffer->Bind();
        hairFrameBuffer->ClearAttachment("Composite", 0, 0, 0, 0);
        hairFrameBuffer->SetViewport();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();
        const RendererSettings& renderSettings = Config::GetRendererSettings();

        // Render all top then all Bottom layers
        RenderHairLayer(drawInfoSet.hairTopLayer, renderSettings.depthPeelCount);
        RenderHairLayer(drawInfoSet.hairBottomLayer, renderSettings.depthPeelCount);

        shader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hairFrameBuffer->GetColorAttachmentHandleByName("Composite"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentHandleByName("FinalLighting"));
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
    }

    void RenderHairLayer(const DrawCommands& drawCommands, int peelCount) {
        const Resolutions& resolutions = Config::GetResolutions();
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* hairFrameBuffer = GetFrameBuffer("Hair");
        OpenGLShader* depthPeelShader = GetShader("HairDepthPeel");
        OpenGLShader* hairLightingShader = GetShader("HairLighting");
        OpenGLShader* hairLayerCompositeShader = GetShader("HairLayerComposite");

        if (!gBuffer) return;
        if (!hairFrameBuffer) return;
        if (!depthPeelShader) return;
        if (!hairLightingShader) return;
        if (!hairLayerCompositeShader) return;

        hairFrameBuffer->Bind();
        hairFrameBuffer->ClearAttachment("ViewspaceDepthPrevious", 1.0f);

        for (int j = 0; j < peelCount; j++) {

            // Viewspace depth pass
            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRenderer::SetViewport(hairFrameBuffer, viewport);
                    OpenGLRenderer::BlitFrameBufferDepth(gBuffer, hairFrameBuffer, viewport);
                    OpenGLRenderer::ClearFrameBufferByViewport(hairFrameBuffer, "ViewspaceDepth", viewport, 0.0f);

                    hairFrameBuffer->Bind();
                    hairFrameBuffer->DrawBuffer("ViewspaceDepth");

                    depthPeelShader->Use();

                    glBindTextureUnit(3, hairFrameBuffer->GetColorAttachmentHandleByName("ViewspaceDepthPrevious"));

                    SetRasterizerState("HairViewspaceDepth");

                    if (BackEnd::RenderDocFound()) {
                        SplitMultiDrawIndirect(depthPeelShader, drawCommands.perViewport[i]);
                    }
                    else {
                        MultiDrawIndirect(drawCommands.perViewport[i]);
                    }
                }
            }
            // Color pass
            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRenderer::SetViewport(hairFrameBuffer, viewport);
                    OpenGLRenderer::ClearFrameBufferByViewport(hairFrameBuffer, "Lighting", viewport, 0.0f, 0.0f, 0.0f, 0.0f);

                    hairFrameBuffer->Bind();
                    hairFrameBuffer->DrawBuffers({ "Lighting", "ViewspaceDepthPrevious" });

                    hairLightingShader->Use();

                    glBindTextureUnit(3, hairFrameBuffer->GetColorAttachmentHandleByName("ViewspaceDepth"));

                    SetRasterizerState("HairLighting");

                    if (BackEnd::RenderDocFound()) {
                        SplitMultiDrawIndirect(hairLightingShader, drawCommands.perViewport[i]);
                    }
                    else {
                        MultiDrawIndirect(drawCommands.perViewport[i]);
                    }
                }
            }
            // Composite
            hairLayerCompositeShader->Use();
            glBindImageTexture(0, hairFrameBuffer->GetColorAttachmentHandleByName("Lighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            glBindImageTexture(1, hairFrameBuffer->GetColorAttachmentHandleByName("Composite"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            int workGroupsX = (hairFrameBuffer->GetWidth() + 7) / 8;
            int workGroupsY = (hairFrameBuffer->GetHeight() + 7) / 8;
            glDispatchCompute(workGroupsX, workGroupsY, 1);
        }
    }
}