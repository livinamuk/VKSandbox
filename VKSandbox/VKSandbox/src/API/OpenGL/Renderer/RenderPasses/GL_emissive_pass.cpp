#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "Viewport/ViewportManager.h"

namespace OpenGLRenderer {

    void EmissivePass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* finalImageFBO = GetFrameBuffer("FinalImage");
        OpenGLShader* horizontalShader = GetShader("BlurHorizontal");
        OpenGLShader* verticalShader = GetShader("BlurVertical");
        OpenGLShader* compositeShader = GetShader("EmissiveComposite");

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glFinish();
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLFrameBuffer* blurBuffer = GetBlurBuffer(i, 0);

            BlitRect srcRect = OpenGLRenderer::BlitRectFromFrameBufferViewport(gBuffer, viewport);
            BlitRect dstRect;
            dstRect.x0 = 0;
            dstRect.x1 = blurBuffer->GetWidth();
            dstRect.y0 = 0;
            dstRect.y1 = blurBuffer->GetHeight();

            OpenGLRenderer::BlitFrameBuffer(gBuffer, blurBuffer, "Emissive", "ColorA", srcRect, dstRect, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            // First round blur (vertical)
            blurBuffer->Bind();
            blurBuffer->SetViewport();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, blurBuffer->GetColorAttachmentHandleByName("ColorA"));
            glDrawBuffer(GL_COLOR_ATTACHMENT1);
            verticalShader->Use();
            verticalShader->SetFloat("targetHeight", blurBuffer->GetHeight());
            DrawQuad();

            for (int j = 1; j < 4; j++) {

                GLuint horizontalSourceHandle = GetBlurBuffer(i, j - 1)->GetColorAttachmentHandleByName("ColorB");
                GLuint verticalSourceHandle = GetBlurBuffer(i, j)->GetColorAttachmentHandleByName("ColorA");

                GetBlurBuffer(i, j)->Bind();
                GetBlurBuffer(i, j)->SetViewport();

                // Second round blur (horizontal)
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, horizontalSourceHandle);
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                horizontalShader->Use();
                horizontalShader->SetFloat("targetWidth", GetBlurBuffer(i, j)->GetWidth());
                DrawQuad();

                // Second round blur (vertical)
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, verticalSourceHandle);
                glDrawBuffer(GL_COLOR_ATTACHMENT1);
                verticalShader->Use();
                verticalShader->SetFloat("targetHeight", GetBlurBuffer(i, j)->GetHeight());
                DrawQuad();
            }

            // Composite those blurred textures into the main lighting image
            float viewportWidth = gBuffer->GetWidth() * viewport->GetSize().x;
            float viewportHeight = gBuffer->GetHeight() * viewport->GetSize().y;
            float viewportOffsetX = gBuffer->GetWidth() * viewport->GetPosition().x;
            float viewportOffsetY = gBuffer->GetHeight() * viewport->GetPosition().y;

            compositeShader->Use();
            compositeShader->SetInt("u_viewportIndex", i);

            glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            glBindTextureUnit(1, GetBlurBuffer(i, 0)->GetColorAttachmentHandleByName("ColorB"));
            glBindTextureUnit(2, GetBlurBuffer(i, 1)->GetColorAttachmentHandleByName("ColorB"));
            glBindTextureUnit(3, GetBlurBuffer(i, 2)->GetColorAttachmentHandleByName("ColorB"));
            glBindTextureUnit(4, GetBlurBuffer(i, 3)->GetColorAttachmentHandleByName("ColorB"));
            glDispatchCompute(viewportWidth / 16, viewportHeight / 4, 1);
        }
    }
}