#include "GL_texture_array.h"
#include "API/OpenGL/GL_util.h"
#include <glad/glad.h>

void OpenGLTextureArray::AllocateMemory(uint32_t width, uint32_t height, uint32_t internalFormat, uint32_t mipmapLevelCount, uint32_t count) {
    if (m_memoryAllocated) return;

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_handle);
    glTextureStorage3D(m_handle, mipmapLevelCount, internalFormat, width, height, count);
    glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, GL_REPEAT);
   
    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_mipmapLevelCount = mipmapLevelCount;
    m_count = count;
    m_memoryAllocated = true;
}

void OpenGLTextureArray::SetLayerDataR16(uint32_t layerIndex, const std::vector<float>& data) {
    if (layerIndex >= m_count) {
        std::cout << "OpenGLTextureArray::SetLayerDataR16(() failed: layerIndex out of range!\n";
        return;
    }
    if (data.size() != static_cast<size_t>(m_width * m_height)) {
        std::cout << "OpenGLTextureArray::SetLayerDataR16() failed because data.size() was " << data.size() << " but texture array width(" << m_width << ") * height(" << m_height << ") equals " << (m_width * m_height) << "\n";
        return;
    }
    glTextureSubImage3D(m_handle, 0, 0, 0, layerIndex, m_width, m_height, 1, GL_RED, GL_FLOAT, data.data());
}

void OpenGLTextureArray::GenerateMipmaps() {
    if (m_memoryAllocated && m_mipmapLevelCount > 1) {
        glGenerateTextureMipmap(m_handle);
    }
}

void OpenGLTextureArray::SetWrapMode(TextureWrapMode wrapMode) {
    if (!m_memoryAllocated) return;
    glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, OpenGLUtil::TextureWrapModeToGLEnum(wrapMode));
    glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, OpenGLUtil::TextureWrapModeToGLEnum(wrapMode));
    glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, OpenGLUtil::TextureWrapModeToGLEnum(wrapMode));
}

void OpenGLTextureArray::SetMinFilter(TextureFilter filter) {
    if (!m_memoryAllocated) return;
    glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, OpenGLUtil::TextureFilterToGLEnum(filter));
}

void OpenGLTextureArray::SetMagFilter(TextureFilter filter) {
    if (!m_memoryAllocated) return;
    glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, OpenGLUtil::TextureFilterToGLEnum(filter));
}