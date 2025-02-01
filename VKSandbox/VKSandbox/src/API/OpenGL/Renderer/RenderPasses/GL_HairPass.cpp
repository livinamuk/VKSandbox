#include "../GL_renderer.h" 
#include "../GL_renderer_util.h" 
#include "../../GL_backend.h"
#include "../Config/Config.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

namespace OpenGLRenderer {

    void HairPass() {
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

        // Cleanup
        gBuffer->SetViewport(); // NOTE!~!! suspect, not necessary if you do shit properly in the next pass
        glDepthFunc(GL_LESS);
    }

    void RenderHairLayer(const DrawCommands& drawCommands, int peelCount) {
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
                    OpenGLRendererUtil::SetViewport(*hairFrameBuffer, *viewport);

                    OpenGLRendererUtil::SetScissor(*hairFrameBuffer, *viewport);

                    OpenGLRendererUtil::BlitFrameBufferDepth(*gBuffer, *hairFrameBuffer, *viewport);

                    hairFrameBuffer->Bind();
                    hairFrameBuffer->ClearAttachment("ViewspaceDepth", 0.0f);
                    hairFrameBuffer->DrawBuffer("ViewspaceDepth");

                    depthPeelShader->Use();

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, hairFrameBuffer->GetColorAttachmentHandleByName("ViewspaceDepthPrevious"));

                    SetRasterizerState("HairViewspaceDepth");
                    MultiDrawIndirect(drawCommands.perViewport[i]);
                }
            }
            // Color pass
            for (int i = 0; i < 4; i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRendererUtil::SetViewport(*hairFrameBuffer, *viewport);
                    OpenGLRendererUtil::SetScissor(*hairFrameBuffer, *viewport);
                    hairFrameBuffer->Bind();
                    hairFrameBuffer->ClearAttachment("Lighting", 0, 0, 0, 0);
                    hairFrameBuffer->DrawBuffers({ "Lighting", "ViewspaceDepthPrevious" });

                    hairLightingShader->Use();

                    glBindTextureUnit(0, hairFrameBuffer->GetColorAttachmentHandleByName("ViewspaceDepth"));

                    SetRasterizerState("HairLighting");
                    MultiDrawIndirect(drawCommands.perViewport[i]);
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
        glDisable(GL_SCISSOR_TEST);
    }
}