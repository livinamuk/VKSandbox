#include "../GL_renderer.h"

#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Editor/Editor.h"

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

        Player* player = Game::GetLocalPlayerByIndex(0);

        // Get me out of here
        // Get me out of here
        // Get me out of here
        lightingShader->SetInt("u_editorLighting", 0);
        lightingShader->SetInt("u_ambientLightingBoost", 0);
        if (Editor::IsOpen()) {
            EditorLightingSettings& lightingSettigns = Editor::GetLightingSettings();
            if (!lightingSettigns.lightingEnabled) {
                lightingShader->SetInt("u_editorLighting", 1);
            }
            lightingShader->SetInt("u_ambientLightingBoost", 1);
        }
        // Get me out of here
        // Get me out of here
        // Get me out of here


        glBindTextureUnit(0, gBuffer->GetColorAttachmentHandleByName("BaseColor"));
        glBindTextureUnit(1, gBuffer->GetColorAttachmentHandleByName("Normal"));
        glBindTextureUnit(2, gBuffer->GetColorAttachmentHandleByName("RMA"));
        glBindTextureUnit(3, gBuffer->GetDepthAttachmentHandle());
        glBindTextureUnit(4, gBuffer->GetColorAttachmentHandleByName("WorldSpacePosition"));
        glBindTextureUnit(5, finalImageFBO->GetColorAttachmentHandleByName("ViewportIndex"));
        glBindTextureUnit(6, gBuffer->GetColorAttachmentHandleByName("Emissive"));
        glBindTextureUnit(7, AssetManager::GetTextureByName("Flashlight2")->GetGLTexture().GetHandle());
        glBindTextureUnit(8, flashLightShadowMapFBO->GetDepthAttachmentHandle());

        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

        glDispatchCompute((gBuffer->GetWidth() + 7) / 8, (gBuffer->GetHeight() + 7) / 8, 1);
    }
}