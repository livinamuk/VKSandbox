#include "../GL_renderer.h"
#include "AssetManagement/AssetManager.h"

namespace OpenGLRenderer {

    void LightingPass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* finalImageFBO = GetFrameBuffer("FinalImage");
        OpenGLFrameBuffer* flashLightShadowMapFBO = GetFrameBuffer("FlashlightShadowMap");
        OpenGLShader* lightingShader = GetShader("Lighting");

        if (!gBuffer) return;
        if (!finalImageFBO) return;
        if (!lightingShader) return;

        lightingShader->Use();

        glBindTextureUnit(0, gBuffer->GetColorAttachmentHandleByName("BaseColor"));
        glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("Normal"));
        glBindTextureUnit(2, gBuffer->GetColorAttachmentHandleByName("RMA"));
        glBindTextureUnit(3, gBuffer->GetDepthAttachmentHandle());
        glBindTextureUnit(4, gBuffer->GetColorAttachmentHandleByName("WorldSpacePosition"));
        glBindTextureUnit(5, finalImageFBO->GetColorAttachmentHandleByName("ViewportIndex"));
        glBindTextureUnit(6, gBuffer->GetColorAttachmentHandleByName("Emissive"));
        glBindTextureUnit(7, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
        glBindTextureUnit(8, flashLightShadowMapFBO->GetDepthAttachmentHandle());
        glBindTextureUnit(9, 339);

        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
    }
}