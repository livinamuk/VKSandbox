#pragma once
#include "HellTypes.h"
#include <string>
#include <vector>

enum class CompressionType { DXT3, BC5, UNDEFINED };

namespace ImageTools {
    // Compresesenator
    void InitializeCMPFramework();
    bool IsCMPFrameworkInitialized();
    void CreateAndExportDDS(const std::string& inputFilepath, const std::string& outputFilepath, bool createMipMaps);

    // Texture Data
    std::vector<TextureData> LoadTextureDataFromDDSThreadSafe(const std::string filepath);
    std::vector<TextureData> LoadTextureDataFromDDSThreadUnsafe(const std::string filepath);
    TextureData LoadUncompressedTextureData(const std::string& filepath);
    TextureData LoadR16FTextureData(const std::string& filepath);
    TextureData LoadEXRData(const std::string& filepath);
    void ConvertRGBA8ToR16F(TextureData& textureData);

    // Util
    void SaveBitmap(const char* filename, unsigned char* data, int width, int height, int numChannels); // problematticly similar to below!!!!

    void SaveBitmap(const std::string& filename, void* data, int width, int height, int internalFormat);
    void SaveHeightMapR16F(const std::string& filename, void* data, int width, int height);
    
    void SaveTextureAsBitmap(const std::vector<std::vector<uint16_t>>& pixels, int width, int height, const std::string& filename);
    void SaveFloatArrayTextureAsBitmap(const std::vector<float>& data, int width, int height, int format, const std::string& filename);

    // Debug
    std::string CMPErrorToString(int error);
    std::string CMPFormatToString(int format);
    int GetChannelCountFromCMPFormat(int format);
}