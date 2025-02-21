#include "GL_util.h"
#include "HellEnums.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace OpenGLUtil {
    bool ExtensionExists(const std::string& extensionName) {
        static std::vector<std::string> extensionsCache;
        if (extensionsCache.empty()) {
            GLint numExtensions;
            glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
            for (GLint i = 0; i < numExtensions; ++i) {
                const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
                extensionsCache.push_back(extension);
            }
        }
        return std::find(extensionsCache.begin(), extensionsCache.end(), extensionName) != extensionsCache.end();
    }

    GLint GetFormatFromChannelCount(int channelCount) {
        switch (channelCount) {
        case 4:  return GL_RGBA;
        case 3:  return GL_RGB;
        case 1:  return GL_RED;
        default:
            std::cout << "Unsupported channel count: " << channelCount << "\n";
            return -1;
        }
    }

    GLint GetInternalFormatFromChannelCount(int channelCount) {
        switch (channelCount) {
        case 4:  return GL_RGBA8;
        case 3:  return GL_RGB8;
        case 1:  return GL_R8;
        default:
            std::cout << "Unsupported channel count: " << channelCount << "\n";
            return -1;
        }
    }

    uint32_t CMPFormatToGLFormat(CMP_FORMAT format) {
        switch (format) {
        case CMP_FORMAT_DXT1:
        case CMP_FORMAT_BC1:
        case CMP_FORMAT_ETC2_RGB:
        case CMP_FORMAT_ETC2_SRGB:
        case CMP_FORMAT_BC6H:
            return GL_RGB;

        case CMP_FORMAT_DXT3:
        case CMP_FORMAT_DXT5:
        case CMP_FORMAT_BC2:
        case CMP_FORMAT_BC7:
        case CMP_FORMAT_ETC2_RGBA:
        case CMP_FORMAT_ETC2_SRGBA:
        case CMP_FORMAT_ASTC:
            return GL_RGBA;

        case CMP_FORMAT_BC4:
        case CMP_FORMAT_ATI2N_XY:
            return GL_RED;

        case CMP_FORMAT_BC5:
            return GL_RG;

        default:
            return 0xFFFFFFFF;
        }
    }

    uint32_t CMPFormatToGLInternalFormat(CMP_FORMAT format) {
        switch (format) {
        case CMP_FORMAT_DXT1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case CMP_FORMAT_DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case CMP_FORMAT_DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case CMP_FORMAT_BC4: return GL_COMPRESSED_RED_RGTC1;
        case CMP_FORMAT_BC5:
        case CMP_FORMAT_ATI2N_XY: return GL_COMPRESSED_RG_RGTC2;
        case CMP_FORMAT_BC6H: return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
        case CMP_FORMAT_BC7: return GL_COMPRESSED_RGBA_BPTC_UNORM;
        case CMP_FORMAT_ETC2_RGB: return GL_COMPRESSED_RGB8_ETC2;
        case CMP_FORMAT_ETC2_SRGB: return GL_COMPRESSED_SRGB8_ETC2;
        case CMP_FORMAT_ETC2_RGBA: return GL_COMPRESSED_RGBA8_ETC2_EAC;
        case CMP_FORMAT_ETC2_SRGBA: return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        case CMP_FORMAT_ASTC: return GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        case CMP_FORMAT_BC1: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case CMP_FORMAT_BC2: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        default: return 0xFFFFFFFF;
        }
    }

    const char* GetGLSyncStatusString(GLenum result) {
        switch (result) {
        case GL_ALREADY_SIGNALED: return "GL_ALREADY_SIGNALED";
        case GL_CONDITION_SATISFIED: return "GL_CONDITION_SATISFIED";
        case GL_TIMEOUT_EXPIRED: return "GL_TIMEOUT_EXPIRED";
        case GL_WAIT_FAILED: return "GL_WAIT_FAILED";
        default: return "UNKNOWN_GL_SYNC_STATUS";
        }
    }

    const char* GLFormatToString(GLenum format) {
        switch (format) {
        case GL_RED: return "GL_RED";
        case GL_RG: return "GL_RG";
        case GL_RGB: return "GL_RGB";
        case GL_BGR: return "GL_BGR";
        case GL_RGBA: return "GL_RGBA";
        case GL_BGRA: return "GL_BGRA";
        default: return "Unknown Format";
        }
    }

    const char* GLInternalFormatToString(GLenum internalFormat) {
        switch (internalFormat) {
        case GL_R8: return "GL_R8";
        case GL_RG8: return "GL_RG8";
        case GL_RGB8: return "GL_RGB8";
        case GL_RGBA8: return "GL_RGBA8";
        case GL_R16F: return "GL_R16F";
        case GL_RG16F: return "GL_RG16F";
        case GL_RGB16F: return "GL_RGB16F";
        case GL_RGBA16F: return "GL_RGBA16F";
        default: return "Unknown Internal Format";
        }
    }

    const char* GLDataTypeToString(GLenum dataType) {
        switch (dataType) {
        case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
        case GL_BYTE: return "GL_BYTE";
        case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
        case GL_SHORT: return "GL_SHORT";
        case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
        case GL_INT: return "GL_INT";
        case GL_HALF_FLOAT: return "GL_HALF_FLOAT";
        case GL_FLOAT: return "GL_FLOAT";
        case GL_DOUBLE: return "GL_DOUBLE";
        default: return "Unknown Data Type";
        }
    }

    GLint GetChannelCountFromFormat(GLenum format) {
        switch (format) {
        case GL_RED: return 1;
        case GL_RG: return 2;
        case GL_RGB: return 3;
        case GL_RGBA: return 4;
        default: return -1;
        }
    }

    size_t CalculateCompressedDataSize(GLenum format, int width, int height) {
        int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT) ? 8 : 16;
        int blocksWide = std::max(1, (width + 3) / 4);
        int blocksHigh = std::max(1, (height + 3) / 4);
        return blocksWide * blocksHigh * blockSize;
    }

    GLint TextureWrapModeToGLEnum(TextureWrapMode wrapMode) {
        switch (wrapMode) {
        case TextureWrapMode::REPEAT: return GL_REPEAT;
        case TextureWrapMode::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case TextureWrapMode::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
        default: return GL_NONE;
        }
    }

    GLint TextureFilterToGLEnum(TextureFilter filter) {
        switch (filter) {
        case TextureFilter::NEAREST: return GL_NEAREST;
        case TextureFilter::LINEAR: return GL_LINEAR;
        case TextureFilter::LINEAR_MIPMAP: return GL_LINEAR_MIPMAP_LINEAR;
        default: return GL_NONE;
        }
    }

    GLenum GLInternalFormatToGLType(GLenum internalFormat) {
        switch (internalFormat) {
            // 8-bit unsigned and signed integer formats
        case GL_R8UI:      return GL_UNSIGNED_BYTE;
        case GL_R8I:       return GL_BYTE;
        case GL_RG8UI:     return GL_UNSIGNED_BYTE;
        case GL_RG8I:      return GL_BYTE;
        case GL_RGBA8UI:   return GL_UNSIGNED_BYTE;
        case GL_RGBA8I:    return GL_BYTE;

            // 16-bit unsigned and signed integer formats
        case GL_R16UI:     return GL_UNSIGNED_SHORT;
        case GL_R16I:      return GL_SHORT;
        case GL_RG16UI:    return GL_UNSIGNED_SHORT;
        case GL_RG16I:     return GL_SHORT;
        case GL_RGBA16UI:  return GL_UNSIGNED_SHORT;
        case GL_RGBA16I:   return GL_SHORT;

            // 32-bit unsigned and signed integer formats
        case GL_R32UI:     return GL_UNSIGNED_INT;
        case GL_R32I:      return GL_INT;
        case GL_RG32UI:    return GL_UNSIGNED_INT;
        case GL_RG32I:     return GL_INT;
        case GL_RGBA32UI:  return GL_UNSIGNED_INT;
        case GL_RGBA32I:   return GL_INT;

            // Special packed integer format
        case GL_RGB10_A2UI: return GL_UNSIGNED_INT_2_10_10_10_REV;

            // Normalized unsigned formats (non-integer)
        case GL_R8:        return GL_UNSIGNED_BYTE;
        case GL_RG8:       return GL_UNSIGNED_BYTE;
        case GL_RGBA8:     return GL_UNSIGNED_BYTE;
        case GL_SRGB8:     return GL_UNSIGNED_BYTE;
        case GL_SRGB8_ALPHA8: return GL_UNSIGNED_BYTE;
        case GL_R16:       return GL_UNSIGNED_SHORT;
        case GL_RG16:      return GL_UNSIGNED_SHORT;
        case GL_RGBA16:    return GL_UNSIGNED_SHORT;

            // Floating point formats
        case GL_R16F:      return GL_FLOAT;
        case GL_RG16F:     return GL_FLOAT;
        case GL_RGBA16F:   return GL_FLOAT;
        case GL_R32F:      return GL_FLOAT;
        case GL_RG32F:     return GL_FLOAT;
        case GL_RGBA32F:   return GL_FLOAT;
            // Depth and depth-stencil formats (if needed)
            // case GL_DEPTH_COMPONENT16:       return GL_UNSIGNED_SHORT;
            // case GL_DEPTH_COMPONENT24:       return GL_UNSIGNED_INT;
            // case GL_DEPTH_COMPONENT32F:      return GL_FLOAT;
            // case GL_DEPTH24_STENCIL8:        return GL_UNSIGNED_INT_24_8;
            // case GL_DEPTH32F_STENCIL8:       return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        default:
            std::cout << "OpenGLUtil::InternalFormatToType(GLenum internalFormat) failed Unsupported internal format\n";
            return 0;
        }
    }

    GLenum GLInternalFormatToGLFormat(GLenum internalFormat) {
        switch (internalFormat) {
            // Red channel formats
        case GL_R8:
        case GL_R8_SNORM:
        case GL_R16:
        case GL_R16_SNORM:
        case GL_R16F:
        case GL_R32F:
            return GL_RED;
        case GL_R8UI:
        case GL_R8I:
        case GL_R16UI:
        case GL_R16I:
        case GL_R32UI:
        case GL_R32I:
            return GL_RED_INTEGER;

            // Red-Green channel formats
        case GL_RG8:
        case GL_RG8_SNORM:
        case GL_RG16:
        case GL_RG16_SNORM:
        case GL_RG16F:
        case GL_RG32F:
            return GL_RG;
        case GL_RG8UI:
        case GL_RG8I:
        case GL_RG16UI:
        case GL_RG16I:
        case GL_RG32UI:
        case GL_RG32I:
            return GL_RG_INTEGER;

            // RGB channel formats
        case GL_RGB8:
        case GL_RGB8_SNORM:
        case GL_RGB16:
        case GL_RGB16_SNORM:
        case GL_RGB16F:
        case GL_RGB32F:
        case GL_SRGB8:
            return GL_RGB;
        case GL_RGB8UI:
        case GL_RGB8I:
        case GL_RGB16UI:
        case GL_RGB16I:
        case GL_RGB32UI:
        case GL_RGB32I:
            return GL_RGB_INTEGER;

            // RGBA channel formats
        case GL_RGBA8:
        case GL_RGBA8_SNORM:
        case GL_RGBA16:
        case GL_RGBA16_SNORM:
        case GL_RGBA16F:
        case GL_RGBA32F:
        case GL_SRGB8_ALPHA8:
            return GL_RGBA;
        case GL_RGBA8UI:
        case GL_RGBA8I:
        case GL_RGBA16UI:
        case GL_RGBA16I:
        case GL_RGBA32UI:
        case GL_RGBA32I:
            return GL_RGBA_INTEGER;

            // Special packed formats
        case GL_RGB10_A2:
        case GL_RGB10_A2UI:
            return GL_RGBA;

            // Depth formats
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:
        case GL_DEPTH_COMPONENT32F:
            return GL_DEPTH_COMPONENT;

            // Depth-stencil formats
        case GL_DEPTH24_STENCIL8:
        case GL_DEPTH32F_STENCIL8:
            return GL_DEPTH_STENCIL;

        default:
            std::cout << "GLInternalFormatToGLFormat: Unsupported internal format\n";
            return 0;
        }
    }
}
