#include "GL_renderer_util.h"
#include "../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"

namespace OpenGLRendererUtil {

    void BlitToDefaultFrameBuffer(GLFrameBuffer& srcFrameBuffer, const char* srcName, GLbitfield mask, GLenum filter) {
        GLint srcAttachmentSlot = srcFrameBuffer.GetColorAttachmentSlotByName(srcName);
        if (srcAttachmentSlot != GL_INVALID_VALUE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glReadBuffer(srcAttachmentSlot);
            glDrawBuffer(GL_BACK);
            BlitRect srcRect;
            srcRect.x0 = 0;
            srcRect.y0 = 0;
            srcRect.x1 = srcFrameBuffer.GetWidth();
            srcRect.y1 = srcFrameBuffer.GetHeight();
            BlitRect dstRect;
            dstRect.y0 = 0;
            dstRect.x0 = 0;
            dstRect.x1 = BackEnd::GetCurrentWindowWidth();
            dstRect.y1 = BackEnd::GetCurrentWindowHeight();
            glBlitFramebuffer(srcRect.x0, srcRect.y0, srcRect.x1, srcRect.y1, dstRect.x0, dstRect.y0, dstRect.x1, dstRect.y1, mask, filter);
        }
    }

    void BlitFrameBuffer(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter) {
        GLint srcAttachmentSlot = srcFrameBuffer.GetColorAttachmentSlotByName(srcName);
        GLint dstAttachmentSlot = dstFrameBuffer.GetColorAttachmentSlotByName(dstName);
        if (srcAttachmentSlot != GL_INVALID_VALUE && dstAttachmentSlot != GL_INVALID_VALUE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer.GetHandle());
            glReadBuffer(srcAttachmentSlot);
            glDrawBuffer(dstAttachmentSlot);
            BlitRect srcRect;
            srcRect.x0 = 0;
            srcRect.y0 = 0;
            srcRect.x1 = srcFrameBuffer.GetWidth();
            srcRect.y1 = srcFrameBuffer.GetHeight();
            BlitRect dstRect;
            dstRect.y0 = 0;
            dstRect.x0 = 0;
            dstRect.x1 = dstFrameBuffer.GetWidth();
            dstRect.y1 = dstFrameBuffer.GetHeight();
            glBlitFramebuffer(srcRect.x0, srcRect.y0, srcRect.x1, srcRect.y1, dstRect.x0, dstRect.y0, dstRect.x1, dstRect.y1, mask, filter);
        }
    }

    void BlitToDefaultFrameBuffer(GLFrameBuffer& srcFrameBuffer, const char* srcName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter) {
        GLint srcAttachmentSlot = srcFrameBuffer.GetColorAttachmentSlotByName(srcName);
        if (srcAttachmentSlot != GL_INVALID_VALUE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glReadBuffer(srcAttachmentSlot);
            glDrawBuffer(GL_BACK);
            glBlitFramebuffer(srcRect.x0, srcRect.y0, srcRect.x1, srcRect.y1, dstRect.x0, dstRect.y0, dstRect.x1, dstRect.y1, mask, filter);
        }
    }

    void BlitFrameBuffer(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, const char* srcName, const char* dstName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask,GLenum filter) {
        GLint srcAttachmentSlot = srcFrameBuffer.GetColorAttachmentSlotByName(srcName);
        GLint dstAttachmentSlot = dstFrameBuffer.GetColorAttachmentSlotByName(dstName);
        if (srcAttachmentSlot != GL_INVALID_VALUE && dstAttachmentSlot != GL_INVALID_VALUE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.GetHandle());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer.GetHandle());
            glReadBuffer(srcAttachmentSlot);
            glDrawBuffer(dstAttachmentSlot);
            glBlitFramebuffer(srcRect.x0, srcRect.y0, srcRect.x1, srcRect.y1, dstRect.x0, dstRect.y0, dstRect.x1, dstRect.y1, mask, filter);
        }
    }

    RenderItem2D CreateRenderItem2D(const std::string& textureName, glm::ivec2 location, glm::ivec2 viewportSize, Alignment alignment, glm::vec3 colorTint, glm::ivec2 size) {
        // Get texture index and dimensions
        Texture* texture = AssetManager::GetTextureByName(textureName);
        if (!texture) {
            std::cout << "RendererUtil::CreateRenderItem2D() failed coz texture is nullptr\n";
            return RenderItem2D();
        }
        float texWidth = (size.x != -1) ? size.x : texture->GetWidth(0);
        float texHeight = (size.y != -1) ? size.y : texture->GetHeight(0);

        // Alignment
        switch (alignment) {
        case Alignment::TOP_LEFT:
            location.x += texWidth * 0.5f;
            location.y += texHeight * 0.5f;
            break;
        case Alignment::TOP_RIGHT:
            location.x -= texWidth * 0.5f;
            location.y += texHeight * 0.5f;
            break;
        case Alignment::BOTTOM_LEFT:
            location.x += texWidth * 0.5f;
            location.y -= texHeight * 0.5f;
            break;
        case Alignment::BOTTOM_RIGHT:
            location.x -= texWidth * 0.5f;
            location.y -= texHeight * 0.5f;
            break;
        }
        // Scale quad to match 1:1 pixel ratio (independent of viewport size)
        float width = texWidth / static_cast<float>(viewportSize.x);
        float height = texHeight / static_cast<float>(viewportSize.y);

        // Calculate final position in normalized device coordinates
        float finalX = (location.x / static_cast<float>(viewportSize.x)) * 2.0f - 1.0f;
        float finalY = 1.0f - (location.y / static_cast<float>(viewportSize.y)) * 2.0f;

        Transform transform;
        transform.position.x = finalX;
        transform.position.y = finalY;
        transform.scale = glm::vec3(width, -height, 1.0f);

        RenderItem2D renderItem;
        renderItem.modelMatrix = transform.to_mat4();
        renderItem.textureIndex = AssetManager::GetTextureIndexByName(textureName);
        renderItem.colorTintR = colorTint.r;
        renderItem.colorTintG = colorTint.g;
        renderItem.colorTintB = colorTint.b;
        return renderItem;
    }

    GLint CreateQuadVAO() {
        GLuint vao = 0;
        GLuint vbo = 0;
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
        };
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
        glBindVertexArray(0);
        return vao;
    }
}