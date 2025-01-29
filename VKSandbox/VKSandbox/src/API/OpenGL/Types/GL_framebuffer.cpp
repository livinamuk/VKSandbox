#include "GL_frameBuffer.h"

void GLFrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}

void GLFrameBuffer::SetViewport() {
    glViewport(0, 0, m_width, m_height);
}

void GLFrameBuffer::Create(const char* name, int width, int height) {
    glCreateFramebuffers(1, &m_handle);
    this->m_name = name;
    this->m_width = width;
    this->m_height = height;
}

void GLFrameBuffer::CleanUp() {
    m_colorAttachments.clear();
    glDeleteFramebuffers(1, &m_handle);
    m_handle = 0;
}

void GLFrameBuffer::Create(const char* name, const glm::ivec2& size) {
    Create(name, size.x, size.y);
}

void GLFrameBuffer::CreateAttachment(const char* name, GLenum internalFormat, GLenum minFilter, GLenum magFilter) {
    ColorAttachment& colorAttachment = m_colorAttachments.emplace_back();
    colorAttachment.name = name;
    colorAttachment.internalFormat = internalFormat;
    glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachment.handle);
    glTextureStorage2D(colorAttachment.handle, 1, internalFormat, m_width, m_height);
    glTextureParameteri(colorAttachment.handle, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(colorAttachment.handle, GL_TEXTURE_MAG_FILTER, magFilter);
    glTextureParameteri(colorAttachment.handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(colorAttachment.handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(m_handle, GL_COLOR_ATTACHMENT0 + m_colorAttachments.size() - 1, colorAttachment.handle, 0); 
    std::string debugLabel = "Texture (FBO: " + std::string(m_name) + " Tex: " + std::string(name) + ")";
    glObjectLabel(GL_TEXTURE, colorAttachment.handle, static_cast<GLsizei>(debugLabel.length()), debugLabel.c_str());

}

void GLFrameBuffer::CreateDepthAttachment(GLenum internalFormat, GLenum minFilter, GLenum magFilter) {
    m_depthAttachment.internalFormat = internalFormat;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment.handle);
    glTextureStorage2D(m_depthAttachment.handle, 1, internalFormat, m_width, m_height);
    glTextureParameteri(m_depthAttachment.handle, GL_TEXTURE_MIN_FILTER, minFilter);
    glTextureParameteri(m_depthAttachment.handle, GL_TEXTURE_MAG_FILTER, magFilter);
    glTextureParameteri(m_depthAttachment.handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_depthAttachment.handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glNamedFramebufferTexture(m_handle, GL_DEPTH_STENCIL_ATTACHMENT, m_depthAttachment.handle, 0);
    std::string debugLabel = "Texture (FBO: " + std::string(m_name) + " Tex: Depth)";
    glObjectLabel(GL_TEXTURE, m_depthAttachment.handle, static_cast<GLsizei>(debugLabel.length()), debugLabel.c_str());
}

bool GLFrameBuffer::StrCmp(const char* queryA, const char* queryB) {
    return std::strcmp(queryA, queryB) == 0;
}


void GLFrameBuffer::DrawBuffers(std::vector<const char*> attachmentNames) {
    std::vector<GLuint> attachments;
    for (const char* attachmentName : attachmentNames) {
        attachments.push_back(GetColorAttachmentSlotByName(attachmentName));
    }
    glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
}

void GLFrameBuffer::DrawBuffer(const char* attachmentName) {
    for (int i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            return;
        }
    }
}

/*void GLFrameBuffer::ClearAttachmentR(const char* attachmentName, float r) {
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            glNamedFramebufferDrawBuffer(m_handle, GL_COLOR_ATTACHMENT0 + i);
            glClearNamedFramebufferfv(m_handle, GL_COLOR, static_cast<GLint>(i), &r);
        }
    }
}
void GLFrameBuffer::ClearAttachmentRTEST(const char* attachmentName, float r) {
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_handle); // Ensure framebuffer is active
            glViewport(0, 0, m_width, m_height);         // Ensure viewport is set

            GLfloat clearValues[1] = { r };

            glNamedFramebufferDrawBuffer(m_handle, GL_COLOR_ATTACHMENT0 + i); // Ensure it's active
            glClearNamedFramebufferfv(m_handle, GL_COLOR, static_cast<GLint>(i), clearValues);
            glNamedFramebufferDrawBuffer(m_handle, GL_NONE); // Restore state

            glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
        }
    }
}

void GLFrameBuffer::ClearAttachmentRG(const char* attachmentName, float r, float g) {
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            const GLfloat clearColor[2] = { r, g };
            glNamedFramebufferDrawBuffer(m_handle, GL_COLOR_ATTACHMENT0 + i);
            glClearNamedFramebufferfv(m_handle, GL_COLOR, static_cast<GLint>(i), clearColor);
        }
    }
}

void GLFrameBuffer::ClearAttachmentRGB(const char* attachmentName, float r, float g, float b) {
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            const GLfloat clearColor[3] = { r, g, b };
            glNamedFramebufferDrawBuffer(m_handle, GL_COLOR_ATTACHMENT0 + i);
            glClearNamedFramebufferfv(m_handle, GL_COLOR, static_cast<GLint>(i), clearColor);
        }
    }
}

void GLFrameBuffer::ClearAttachmentRGBA(const char* attachmentName, float r, float g, float b, float a) {
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            const GLfloat clearColor[4] = { r, g, b, a };
            glNamedFramebufferDrawBuffer(m_handle, GL_COLOR_ATTACHMENT0 + i);
            glClearNamedFramebufferfv(m_handle, GL_COLOR, static_cast<GLint>(i), clearColor);
        }
    }
}
*/

void GLFrameBuffer::ClearAttachment(const char* attachmentName, float r, float g, float b, float a) {
    for (int i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(attachmentName, m_colorAttachments[i].name)) {
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT);
            return;
        }
    }
}

void GLFrameBuffer::ClearDepthAttachment() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

GLuint GLFrameBuffer::GetHandle() const {
    return m_handle;
}

GLuint GLFrameBuffer::GetWidth() const {
    return m_width;
}

GLuint GLFrameBuffer::GetHeight() const {
    return m_height;
}

GLuint GLFrameBuffer::GetColorAttachmentHandleByName(const char* name) const {
    for (int i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(name, m_colorAttachments[i].name)) {
            return m_colorAttachments[i].handle;
        }
    }
    std::cerr << "GetColorAttachmentHandleByName() with name '" << name << "' failed. Name does not exist in FrameBuffer '" << this->m_name << "'\n";
    return GL_NONE;
}

GLuint GLFrameBuffer::GetDepthAttachmentHandle() const {
    return m_depthAttachment.handle;
}

GLenum GLFrameBuffer::GetColorAttachmentSlotByName(const char* name) const {
    for (int i = 0; i < m_colorAttachments.size(); i++) {
        if (StrCmp(name, m_colorAttachments[i].name)) {
            return GL_COLOR_ATTACHMENT0 + i;
        }
    }
    std::cerr << "GetColorAttachmentSlotByName() with name '" << name << "' failed. Name does not exist in FrameBuffer '" << this->m_name << "'\n";
    return GL_INVALID_VALUE;
}

void GLFrameBuffer::BlitToDefaultFrameBuffer(const char* srcName, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, GetHandle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glReadBuffer(GetColorAttachmentSlotByName(srcName));
    glDrawBuffer(GL_BACK);
    glBlitFramebuffer(0, 0, GetWidth(), GetHeight(), dstX0, dstY0, dstX1, dstY1, mask, filter);
}
