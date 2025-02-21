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

void OpenGLTextureArray::LoadTextureData(TextureData& textureData, uint32_t arrayIndex) {
    if (!m_memoryAllocated) {
        std::cout << "OpenGLTextureArray::LoadTextureData() failed: Memory not allocated for OpenGLTextureArray\n";
        return;
    }
    if (arrayIndex >= m_count) {
        std::cout << "OpenGLTextureArray::LoadTextureData() failed: Slot index out of bounds\n";
        return;
    }
    if (textureData.m_internalFormat != m_internalFormat) {
        std::cout << "OpenGLTextureArray::LoadTextureData() failed: texture data internal format does not match allocated memory internal format\n";
        return;
    }
    if (textureData.m_imageDataType == ImageDataType::COMPRESSED) {
        std::cout << "OpenGLTextureArray::LoadTextureData() failed: compressed texture data currently unsupported\n";
        return;
    }
    if (textureData.m_imageDataType == ImageDataType::EXR) {
        std::cout << "OpenGLTextureArray::LoadTextureData() failed: exr texture data currently unsupported\n";
        return;
    }

    // Determine format and data type
    GLenum format = (textureData.m_channelCount == 4) ? GL_RGBA :
        (textureData.m_channelCount == 3) ? GL_RGB :
        (textureData.m_channelCount == 2) ? GL_RG : GL_RED;

    GLenum dataType = GL_UNSIGNED_BYTE; // Default to 8-bit

    if (textureData.m_internalFormat == GL_R16F) {
        dataType = GL_HALF_FLOAT;  // Correct type for 16-bit floating-point textures
    }
    else if (textureData.m_internalFormat == GL_R16) {
        dataType = GL_UNSIGNED_SHORT;  // Correct type for 16-bit unsigned integer textures
    }

    if ((textureData.m_channelCount == 4 && format != GL_RGBA) ||
        (textureData.m_channelCount == 3 && format != GL_RGB) ||
        (textureData.m_channelCount == 2 && format != GL_RG) ||
        (textureData.m_channelCount == 1 && format != GL_RED)) {
        std::cout << "OpenGLTextureArray::LoadTextureData() failed: Texture channel count (" << textureData.m_channelCount << ") does not match expected OpenGL format\n";
        return;
    }
    //std::cout << "LoadTextureData() format: " << OpenGLUtil::GLFormatToString(format) << "\n";
    //std::cout << "LoadTextureData() dataType: " << OpenGLUtil::GLDataTypeToString(dataType) << "\n";
    glTextureSubImage3D(m_handle, 0, 0, 0, arrayIndex, textureData.m_width, textureData.m_height, 1, format, dataType, textureData.m_data);
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