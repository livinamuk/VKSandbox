#include "Util.h"
#include "API/OpenGL/GL_util.h"

namespace Util {

    void PrintDebugInfo(TextureData& textureData) {
        std::cout << "Width: " << textureData.m_width << "\n";
        std::cout << "Height: " << textureData.m_height << "\n";
        std::cout << "Channel Count: " << textureData.m_channelCount << "\n";
        std::cout << "Data Size: " << textureData.m_dataSize << " bytes\n";
        std::cout << "Format: " << OpenGLUtil::GLFormatToString(textureData.m_format) << "\n";
        std::cout << "Internal Format: " << OpenGLUtil::GLInternalFormatToString(textureData.m_internalFormat) << "\n";
        std::cout << "Image Data Type: " << Util::ImageDataTypeToString(textureData.m_imageDataType) << "\n";
        std::cout << "Data Pointer: " << textureData.m_data << "\n";
    }

    std::string BytesToMBString(size_t bytes) {
        double megabytes = bytes / (1024.0 * 1024.0);
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << megabytes << " MB";
        return stream.str();
    }
}