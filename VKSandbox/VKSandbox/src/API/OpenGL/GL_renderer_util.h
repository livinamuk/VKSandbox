#pragma once
#include "HellTypes.h"
#include "Types/GL_frameBuffer.hpp"

namespace OpenGLRendererUtil {

    void BlitToDefaultFrameBuffer(
        GLFrameBuffer& srcFrameBuffer,
        const char* srcName,
        GLbitfield mask,
        GLenum filter
    );

    void BlitFrameBuffer(
        GLFrameBuffer& srcFrameBuffer,
        GLFrameBuffer& dstFrameBuffer,
        const char* srcName,
        const char* dstName,
        GLbitfield mask,
        GLenum filter
    );

    void BlitToDefaultFrameBuffer(
        GLFrameBuffer& srcFrameBuffer, 
        const char* srcName, 
        BlitRect srcRect, 
        BlitRect dstRect, 
        GLbitfield mask, 
        GLenum filter
    );

    void BlitFrameBuffer(
        GLFrameBuffer& srcFrameBuffer, 
        GLFrameBuffer& dstFrameBuffer,
        const char* srcName,
        const char* dstName,
        BlitRect srcRect, 
        BlitRect dstRect, 
        GLbitfield mask, 
        GLenum filter
    );

    RenderItem2D CreateRenderItem2D(
        const std::string& textureName,
        glm::ivec2 location,
        glm::ivec2 viewportSize,
        Alignment alignment,
        glm::vec3 colorTint = WHITE,
        glm::ivec2 size = glm::ivec2(-1, -1)
    );

    GLint CreateQuadVAO();

}