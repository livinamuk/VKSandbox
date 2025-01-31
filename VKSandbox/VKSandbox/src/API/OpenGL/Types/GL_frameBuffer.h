#pragma once
#include <glad/glad.h>
#include "../GL_Util.h"
#include <vector>
#include <iostream>
#include <cstring>
#include "HellTypes.h"

struct ColorAttachment {
    const char* name = "undefined";
    GLuint handle = 0;
    GLenum internalFormat = GL_RGBA;
};

struct DepthAttachment {
    GLuint handle = 0;
    GLenum internalFormat = GL_RGBA;
};

class OpenGLFrameBuffer {
private:
    const char* m_name = "undefined";
    GLuint m_handle = 0;
    GLuint m_width = 0;
    GLuint m_height = 0;
    std::vector<ColorAttachment> m_colorAttachments;
    DepthAttachment m_depthAttachment;

    static bool StrCmp(const char* queryA, const char* queryB);

public:
    OpenGLFrameBuffer() = default;
    OpenGLFrameBuffer(const char* name, int width, int height);
    OpenGLFrameBuffer(const char* name, const glm::ivec2& resolution);
    void Create(const char* name, int width, int height);
    void Create(const char* name, const glm::ivec2& resolution);
    void CleanUp();
    void CreateAttachment(const char* name, GLenum internalFormat, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR);
    void CreateDepthAttachment(GLenum internalFormat, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR);
    void Bind();
    void SetViewport();
    void DrawBuffers(std::vector<const char*> attachmentNames);
    void DrawBuffer(const char* attachmentName);
    //void ClearAttachmentR(const char* attachmentName, float r);
    //void ClearAttachmentRTEST(const char* attachmentName, float r);
    //void ClearAttachmentRG(const char* attachmentName, float r, float g);
    //void ClearAttachmentRGB(const char* attachmentName, float r, float g, float b);
    //void ClearAttachmentRGBA(const char* attachmentName, float r, float g, float b, float a);
    void ClearAttachment(const char* attachmentName, float r, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    void ClearDepthAttachment();
    GLuint GetHandle() const;
    GLuint GetWidth() const;
    GLuint GetHeight() const;
    GLuint GetColorAttachmentHandleByName(const char* name) const;
    GLuint GetDepthAttachmentHandle() const;
    GLenum GetColorAttachmentSlotByName(const char* name) const;
    void BlitToDefaultFrameBuffer(const char* srcName, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
};
