#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../GL_util.hpp"

struct ColorAttachment {
    const char* name = "undefined";
    GLuint handle = 0;
    GLenum internalFormat = GL_RGBA;
};
struct DepthAttachment {
    GLuint handle = 0;
    GLenum internalFormat = GL_RGBA;
};

struct GLFrameBuffer {

private:
    const char* m_name = "undefined";
    GLuint m_handle = 0;
    GLuint m_width = 0;
    GLuint m_height = 0;
    std::vector<ColorAttachment> m_colorAttachments;
    DepthAttachment m_depthAttachment;

public:

    static bool StrCmp(const char* queryA, const char* queryB) {
        if (strcmp(queryA, queryB) == 0)
            return true;
        else
            return false;
    }

    void Create(const char* name, int width, int height) {
        glGenFramebuffers(1, &m_handle);
        this->m_name = name;
        this->m_width = width;
        this->m_height = height;
    }

    void CleanUp() {
        m_colorAttachments.clear();
        glDeleteFramebuffers(1, &m_handle);
    }

    void CreateAttachment(const char* name, GLenum internalFormat, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR) {
        GLenum slot = GL_COLOR_ATTACHMENT0 + m_colorAttachments.size();
        ColorAttachment& colorAttachment = m_colorAttachments.emplace_back();
        colorAttachment.name = name;
        colorAttachment.internalFormat = internalFormat;
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
        glGenTextures(1, &colorAttachment.handle);
        glBindTexture(GL_TEXTURE_2D, colorAttachment.handle);
        glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, m_width, m_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, slot, GL_TEXTURE_2D, colorAttachment.handle, 0);
    }

    void CreateDepthAttachment(GLenum internalFormat, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR) {
        m_depthAttachment.internalFormat = internalFormat;
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
        glGenTextures(1, &m_depthAttachment.handle);
        glBindTexture(GL_TEXTURE_2D, m_depthAttachment.handle);
        glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, m_width, m_height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment.handle, 0);
    }

    void Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    }

    void SetViewport() {
        glViewport(0, 0, m_width, m_height);
    }

    void DrawBuffers(std::vector<const char*> attachmentNames) {
        std::vector<GLuint> attachments;
        for (const char* attachmentName : attachmentNames) {
            attachments.push_back(GetColorAttachmentSlotByName(attachmentName));
        }
        glDrawBuffers(attachments.size(), &attachments[0]);
    }

    void DrawBuffer(const char* attachmentName) {
        for (int i = 0; i < m_colorAttachments.size(); i++) {
            if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
                glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
                return;
            }
        }
    }

    void ClearAttachment(const char* attachmentName, float r, float g, float b, float a) {
        for (int i = 0; i < m_colorAttachments.size(); i++) {
            if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
                glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
                glClearColor(r, g, b, a);
                glClear(GL_COLOR_BUFFER_BIT);
                glDrawBuffer(GL_NONE);
                return;
            }
        }
    }

    void ClearDepthAttachment() {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    GLuint GetHandle() {
        return m_handle;
    }

    GLuint GetWidth() {
        return m_width;
    }

    GLuint GetHeight() {
        return m_height;
    }

    GLuint GetColorAttachmentHandleByName(const char* name) {
        for (int i = 0; i < m_colorAttachments.size(); i++) {
            if (StrCmp(name, m_colorAttachments[i].name)) {
                return m_colorAttachments[i].handle;
            }
        }
        std::cout << "GetColorAttachmentHandleByName() with name '" << name << "' failed. Name does not exist in FrameBuffer '" << this->m_name << "'\n";
        return GL_NONE;
    }

    GLuint GetDepthAttachmentHandle() {
        return m_depthAttachment.handle;
    }

    GLenum GetColorAttachmentSlotByName(const char* name) {
        for (int i = 0; i < m_colorAttachments.size(); i++) {
            if (StrCmp(name, m_colorAttachments[i].name)) {
                return GL_COLOR_ATTACHMENT0 + i;
            }
        }
        std::cout << "GetColorAttachmentHandleByName() with name '" << name << "' failed. Name does not exist in FrameBuffer '" << this->m_name << "'\n";
        return GL_INVALID_VALUE;
    }

    void BlitToDefaultFrameBuffer(const char* srcName, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, GetHandle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glReadBuffer(GetColorAttachmentSlotByName(srcName));
        glDrawBuffer(GL_BACK);
        glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), dstX0, dstY0, dstX1, dstY1, mask, filter);
    }
};

