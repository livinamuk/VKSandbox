#pragma once
#include <glad/glad.h>
#include <cstdint>

struct OpenGLSSBO {
public:

    OpenGLSSBO() = default;

    OpenGLSSBO(size_t size, GLbitfield flags) {
        m_flags = flags;
        PreAllocate(size);
    }

    uint32_t GetHandle() const {
        return m_handle;
    }

    void PreAllocate(size_t size) {
        CleanUp();
        glCreateBuffers(1, &m_handle);
        glNamedBufferStorage(m_handle, (GLsizeiptr)size, nullptr, m_flags);
        m_bufferSize = size;
    }

    void Update(size_t size, const void* data) {
        if (size == 0 || data == nullptr) {
            return;
        }
        if (m_handle == 0 || m_bufferSize < size) {
            // Destroy old buffer and allocate a new one
            CleanUp();
            glCreateBuffers(1, &m_handle);
            glNamedBufferStorage(m_handle, (GLsizeiptr)size, nullptr, m_flags);
            m_bufferSize = size;
        }
        // Upload data
        glNamedBufferSubData(m_handle, 0, (GLsizeiptr)size, data);
    }

    void Bind(GLint index) const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_handle);
    }

    void CleanUp() {
        if (m_handle != 0) {
            glDeleteBuffers(1, &m_handle);
            m_handle = 0;
            m_bufferSize = 0;
        }
    }

private:
    GLbitfield m_flags = 0;
    uint32_t m_handle = 0;
    size_t m_bufferSize = 0;
};
