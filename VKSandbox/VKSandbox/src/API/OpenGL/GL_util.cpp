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
}
