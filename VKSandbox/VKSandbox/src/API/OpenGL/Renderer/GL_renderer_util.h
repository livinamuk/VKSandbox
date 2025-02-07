#pragma once
#include "HellTypes.h"
#include "HellDefines.h"
#include "../Types/GL_frameBuffer.h"
#include "../Viewport/Viewport.h"

namespace OpenGLRendererUtil {
    void SetViewport(OpenGLFrameBuffer* framebuffer, Viewport* viewport);
    void ClearFrameBufferByViewport(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLfloat r, GLfloat g = 0.0f, GLfloat b = 0.0f, GLfloat a = 0.0f);
    void ClearFrameBufferByViewportInt(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLint r, GLint g = 0.0f, GLint b = 0.0f, GLint a = 0.0f);
    void ClearFrameBufferByViewportUInt(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLuint r, GLuint g = 0.0f, GLuint b = 0.0f, GLuint a = 0.0f);
    void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);
    void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const Viewport* viewport);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, BlitRect srcRect, BlitRect dstRect);
    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* srcName, GLbitfield mask, GLenum filter);
    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* srcName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    RenderItem2D CreateRenderItem2D(const std::string& textureName, glm::ivec2 location, glm::ivec2 viewportSize, Alignment alignment, glm::vec3 colorTint = WHITE, glm::ivec2 size = glm::ivec2(-1, -1));
    GLint CreateQuadVAO();
}