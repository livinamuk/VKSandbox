#pragma once
#include "HellTypes.h"
#include "../Types/GL_frameBuffer.h"
#include "../Viewport/Viewport.h"

namespace OpenGLRendererUtil {
    void SetViewport(const GLFrameBuffer& framebuffer, const Viewport& viewport);
    void SetScissor(const GLFrameBuffer& framebuffer, const Viewport& viewport);
    void BlitFrameBuffer(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);
    void BlitFrameBuffer(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, const char* srcName, const char* dstName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    void BlitFrameBufferDepth(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer);
    void BlitFrameBufferDepth(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, const Viewport& viewport);
    void BlitFrameBufferDepth(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, BlitRect srcRect, BlitRect dstRect);
    void BlitToDefaultFrameBuffer(GLFrameBuffer& srcFrameBuffer, const char* srcName, GLbitfield mask, GLenum filter);
    void BlitToDefaultFrameBuffer(GLFrameBuffer& srcFrameBuffer, const char* srcName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    RenderItem2D CreateRenderItem2D(const std::string& textureName, glm::ivec2 location, glm::ivec2 viewportSize, Alignment alignment, glm::vec3 colorTint = WHITE, glm::ivec2 size = glm::ivec2(-1, -1));
    GLint CreateQuadVAO();
}