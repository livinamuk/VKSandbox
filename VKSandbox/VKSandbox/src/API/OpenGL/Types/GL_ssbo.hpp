#include <glad/glad.h>
#include <cstdint>

struct SSBO {
public:
    uint32_t GetHandle() {
        return m_handle;
    }
    
    void PreAllocate(size_t size) {
        glCreateBuffers(1, &m_handle);
        glNamedBufferStorage(m_handle, (GLsizeiptr)size, NULL, GL_DYNAMIC_STORAGE_BIT);
        m_bufferSize = size;
    }

    void Update(size_t size, void* data) {
        if (size <= 0) {
            return;
        }
        if (m_handle == 0) {
            glCreateBuffers(1, &m_handle);
            glNamedBufferStorage(m_handle, (GLsizeiptr)size, NULL, GL_DYNAMIC_STORAGE_BIT);
            m_bufferSize = size;
        }
        if (m_bufferSize < size) {
            glDeleteBuffers(1, &m_handle);
            glCreateBuffers(1, &m_handle);
            glNamedBufferStorage(m_handle, (GLsizeiptr)size, NULL, GL_DYNAMIC_STORAGE_BIT);
            m_bufferSize = size;
        }
        glNamedBufferSubData(m_handle, 0, (GLsizeiptr)size, data);
    }

    void Bind(GLint index) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_handle);
    }

    void CleanUp() {
        if (m_handle != 0) {
            glDeleteBuffers(1, &m_handle);
        }
    }

private:
    uint32_t m_handle = 0;
    size_t m_bufferSize = 0;
};