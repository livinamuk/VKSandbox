#include "ImageTools.h"
#include "HellTypes.h"
#include "HellEnums.h"
#include <stdio.h>
#include <memory.h>
#include <iostream>
#include <filesystem>
#pragma warning(push)
#pragma warning(disable : 4996)
#include "stb_image_write.h"
#pragma warning(pop)
#include "tinyexr.h"
#include "../API/OpenGL/GL_Util.h" // Remove me when you can
#include "cmp_compressonatorlib/compressonator.h"
#include <mutex>
#include <fstream>
#include "DDS.h"
#include <stb_image.h>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <bitset>  // For binary debugging
#include <lodepng/lodepng.h>

namespace ImageTools {

    bool m_CMPFrameworkInitilized = false;

    void InitializeCMPFramework() {
        CMP_InitFramework();
        m_CMPFrameworkInitilized = true;
    }

    bool IsCMPFrameworkInitialized() {
        return m_CMPFrameworkInitilized;
    }
    bool CompressionCallback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
        (pUser1);
        (pUser2);
        std::printf("\rCompression progress = %3.0f", fProgress);
        return false;
    }

    std::vector<TextureData> LoadTextureDataFromDDSThreadSafe(const std::string filepath) {
        std::vector<TextureData> textureDataLevels;

        // Open the file in binary mode
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cout << "Failed to open DDS file: " << filepath << "\n";
            return textureDataLevels;
        }
        // Read and validate the DDS header
        DDSHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (header.dwMagic != 0x20534444) { // "DDS " magic number
            std::cout << "Not a valid DDS file: " << filepath << "\n";
            return textureDataLevels;
        }
        // Check for potential DX10 extended header
        DDSHeaderDX10 dx10Header = {};
        if (header.ddspf_dwFourCC == 0x30315844) { // "DX10" FourCC
            file.read(reinterpret_cast<char*>(&dx10Header), sizeof(dx10Header));
        }
        // Retrieve format information
        DDSFormatInfo formatInfo = GetDDSFormatInfo(header, &dx10Header);

        // Iterate the mipmap levels
        uint32_t mipWidth = header.dwWidth;
        uint32_t mipHeight = header.dwHeight;
        for (uint32_t i = 0; i < header.dwMipMapCount; ++i) {
            uint32_t blocksWide = (mipWidth + 3) / 4;
            uint32_t blocksHigh = (mipHeight + 3) / 4;
            uint32_t dataSize = blocksWide * blocksHigh * formatInfo.blockSize;

            // Read the mipmap data
            std::vector<char> buffer(dataSize);
            file.read(buffer.data(), dataSize);
            if (file.gcount() != static_cast<std::streamsize>(dataSize)) {
                std::cerr << "Error reading mip level " << i << "\n";
                break;
            }
            // Store the mipmap data
            TextureData& textureData = textureDataLevels.emplace_back();
            textureData.m_dataSize = dataSize;
            textureData.m_data = new char[dataSize];
            std::memcpy(textureData.m_data, buffer.data(), dataSize);
            textureData.m_width = mipWidth;
            textureData.m_height = mipHeight;
            textureData.m_internalFormat = formatInfo.internalFormat;
            textureData.m_format = formatInfo.format;
            textureData.m_channelCount = formatInfo.channelCount;
            mipWidth = std::max(1u, mipWidth / 2);
            mipHeight = std::max(1u, mipHeight / 2);
        }

        file.close();
        return textureDataLevels;
    }

    std::vector<TextureData> LoadTextureDataFromDDSThreadUnsafe(const std::string filepath) {
        if (!IsCMPFrameworkInitialized) {
            InitializeCMPFramework();
        }
        std::vector<TextureData> textureDataLevels;
        CMP_MipSet mipSetIn = {};
        CMP_MipSet mipSetOut = {};
        CMP_ERROR status;
        KernelOptions kernelOptions = {};

        // Load the texture
        status = CMP_LoadTexture(filepath.c_str(), &mipSetIn);
        if (status != CMP_OK) {
            std::cout << "Error: Failed to load texture. Error code: " << CMPErrorToString(status) << "\n";
            return textureDataLevels;
        }
        // Manually populate missing miplevel data. This probably isn't required,
        // but you could not figure out how to populate or access it any other way.
        int currentOffset = 0;
        for (int i = 0; i < mipSetIn.m_nMipLevels; i++) {
            CMP_MipLevel* mipLevelData = mipSetIn.m_pMipLevelTable[i];
            int mipWidth = std::max(static_cast<int>(mipSetIn.dwWidth >> i), 1);
            int mipHeight = std::max(static_cast<int>(mipSetIn.m_nHeight >> i), 1);
            int blockSize = (mipSetIn.m_format == CMP_FORMAT_BC1) ? 8 : 16;
            int rowBytes = ((mipWidth + 3) / 4) * blockSize;
            int dataSize = rowBytes * ((mipHeight + 3) / 4);
            mipLevelData->m_nWidth = mipWidth;
            mipLevelData->m_nHeight = mipHeight;
            mipLevelData->m_pbData = mipSetIn.pData + currentOffset;
            mipLevelData->m_dwLinearSize = dataSize;
            currentOffset += dataSize;
        }
        // Fill the TextureData struct for this mipmap level
        for (int i = 0; i < mipSetIn.m_nMipLevels; i++) {
            CMP_MipLevel* mipLevelData = mipSetIn.m_pMipLevelTable[i];
            TextureData& textureData = textureDataLevels.emplace_back();
            textureData.m_data = mipLevelData->m_pbData;
            textureData.m_width = mipLevelData->m_nWidth;
            textureData.m_height = mipLevelData->m_nHeight;
            textureData.m_dataSize = mipLevelData->m_dwLinearSize;
            textureData.m_internalFormat = OpenGLUtil::CMPFormatToGLInternalFormat(mipSetIn.m_format);
            textureData.m_format = OpenGLUtil::CMPFormatToGLFormat(mipSetIn.m_format);
            textureData.m_channelCount = OpenGLUtil::GetChannelCountFromFormat(mipSetIn.m_format);
        }
        return textureDataLevels;
    }

    void CreateAndExportDDS(const std::string& inputFilepath, const std::string& outputFilepath, bool generateMipMaps) {
        if (!IsCMPFrameworkInitialized) {
            InitializeCMPFramework();
        }
        CMP_MipSet mipSetIn = {};
        CMP_MipSet mipSetOut = {};
        KernelOptions kernelOptions = {};
        CMP_ERROR status;

        // Load the texture
        status = CMP_LoadTexture(inputFilepath.c_str(), &mipSetIn);
        if (status != CMP_OK) {
            std::cout << "Error: Failed to load texture. Error code: " << CMPErrorToString(status) << "\n";
            return;
        }
        // Generate mipmaps
        if (generateMipMaps) {
            CMP_INT mipmapLevelCount = (CMP_INT)(std::log2(std::max(mipSetIn.m_nWidth, mipSetIn.m_nHeight))) + 1;
            CMP_INT minSize = CMP_CalcMinMipSize(mipSetIn.m_nHeight, mipSetIn.m_nWidth, mipmapLevelCount);
            CMP_GenerateMIPLevels(&mipSetIn, minSize);
        }
        // Compression settings
        kernelOptions.encodeWith = CMP_HPC; // CMP_CPU // CMP_GPU_OCL
        kernelOptions.format = CMP_FORMAT_BC7;
        kernelOptions.fquality = 0.88;
        kernelOptions.threads = 0;
       
        memset(&mipSetOut, 0, sizeof(CMP_MipSet));
        status = CMP_ProcessTexture(&mipSetIn, &mipSetOut, kernelOptions, CompressionCallback);
        std::cout << "\n";
        if (status != CMP_OK) {
            std::cout << "Failed to process texture " << inputFilepath << ": " << CMPErrorToString(status) << "\n";
            return;
        }
        status = CMP_SaveTexture(outputFilepath.c_str(), &mipSetOut);
        if (status != CMP_OK) {
            CMP_FreeMipSet(&mipSetIn);
            std::cout << "Failed to save texture " << inputFilepath << ": " << CMPErrorToString(status) << "\n";
            return;
        }
        // Cleanup
        CMP_FreeMipSet(&mipSetIn);
        CMP_FreeMipSet(&mipSetOut);
    }

    TextureData LoadUncompressedTextureData(const std::string& filepath) {
        stbi_set_flip_vertically_on_load(false);
        TextureData textureData;
        uint8_t* imageData = stbi_load(filepath.data(), &textureData.m_width, &textureData.m_height, &textureData.m_channelCount, 0);
        textureData.m_imageDataType = ImageDataType::UNCOMPRESSED;

        if (textureData.m_channelCount == 3) {
            size_t newSize = textureData.m_width * textureData.m_height * 4;
            uint8_t* rgbaData = new uint8_t[newSize];
            for (size_t i = 0, j = 0; i < newSize; i += 4, j += 3) {
                rgbaData[i] = imageData[j];         // R
                rgbaData[i + 1] = imageData[j + 1]; // G
                rgbaData[i + 2] = imageData[j + 2]; // B
                rgbaData[i + 3] = 255;              // A
            }
            stbi_image_free(imageData);
            textureData.m_data = rgbaData;
            textureData.m_channelCount = 4;
        }
        else {
            textureData.m_data = imageData;
        }

        // If mipmaps are requested, allocate space for them
        if (textureData.m_channelCount == 4 && textureData.m_width != textureData.m_height) {
            textureData.m_dataSize = textureData.m_width * textureData.m_height * 4;
        }
        else {
            textureData.m_dataSize = textureData.m_width * textureData.m_height * textureData.m_channelCount;
        }

        textureData.m_format = OpenGLUtil::GetFormatFromChannelCount(textureData.m_channelCount);
        textureData.m_internalFormat = OpenGLUtil::GetInternalFormatFromChannelCount(textureData.m_channelCount);

        return textureData;
    }


    TextureData LoadEXRData(const std::string& filepath) {
        TextureData textureData;
        textureData.m_imageDataType = ImageDataType::EXR;
        const char* err = nullptr;
        const char** layer_names = nullptr;
        int num_layers = 0;
        bool status = EXRLayers(filepath.c_str(), &layer_names, &num_layers, &err);
        free(layer_names);
        const char* layername = NULL;
        float* floatPtr = nullptr;
        status = LoadEXRWithLayer(&floatPtr, &textureData.m_width, &textureData.m_height, filepath.c_str(), layername, &err);
        textureData.m_data = floatPtr;
        textureData.m_channelCount = -1; // TODO
        textureData.m_dataSize = -1; // TODO
        textureData.m_format = -1; // TODO
        textureData.m_internalFormat = -1; // TODO
        return textureData;
    }

    float HalfToFloat(uint16_t h) {
        uint32_t sign = (h & 0x8000) << 16;      // Extract sign bit
        uint32_t exponent = (h & 0x7C00) >> 10;  // Extract exponent
        uint32_t mantissa = (h & 0x03FF);        // Extract mantissa

        if (exponent == 0) {
            if (mantissa == 0) {
                float zero = (sign != 0) ? -0.0f : 0.0f;
                return zero;
            }
            float subnormal = std::ldexp(static_cast<float>(mantissa), -24);
            return subnormal;
        }

        if (exponent == 31) {
            float special = (mantissa == 0) ? std::copysign(INFINITY, sign) : NAN;
            return special;
        }

        exponent = exponent - 15 + 127;  // Convert exponent from half-float to full-float
        uint32_t floatBits = sign | (exponent << 23) | (mantissa << 13);

        float result;
        std::memcpy(&result, &floatBits, sizeof(result));  // Bitcast to float

        return result;
    }

    uint16_t FloatToHalf(float value) {
        uint32_t f = *(uint32_t*)&value; // Interpret float as uint32_t
        uint32_t sign = (f >> 16) & 0x8000; // Extract sign bit
        uint32_t exponent = (f >> 23) & 0xFF; // Extract exponent
        uint32_t mantissa = f & 0x7FFFFF; // Extract mantissa

        if (exponent == 255) { // Inf or NaN
            if (mantissa) return sign | 0x7E00; // NaN
            return sign | 0x7C00; // Infinity
        }

        if (exponent > 142) { // Too large, return infinity
            return sign | 0x7C00;
        }

        if (exponent < 113) { // Too small, round to zero or denormalized
            if (exponent < 103) return sign; // Underflow to zero
            mantissa |= 0x800000; // Set implicit leading 1
            mantissa >>= (113 - exponent); // Shift to denormalized range
            return sign | (mantissa >> 13);
        }

        return sign | ((exponent - 112) << 10) | (mantissa >> 13);
    }

    TextureData LoadR16FTextureData(const std::string& filepath) {
        stbi_set_flip_vertically_on_load(false);
        TextureData textureData;

        int width, height, channels;
        uint16_t* imageData = stbi_load_16(filepath.c_str(), &width, &height, &channels, 1); // Force single-channel

        if (!imageData) {
            std::cout << "[LoadR16FTextureData] Failed to load R16F texture: " << filepath << "\n";
            return textureData;
        }

        size_t pixelCount = width * height;
        std::vector<uint16_t> halfFloatData(imageData, imageData + pixelCount); // Store raw uint16_t

        // Print min/max values for debugging
        uint16_t minVal = *std::min_element(halfFloatData.begin(), halfFloatData.end());
        uint16_t maxVal = *std::max_element(halfFloatData.begin(), halfFloatData.end());
        std::cout << "[LoadR16FTextureData] Min: " << minVal << ", Max: " << maxVal << "\n";

        // Print first 10 pixel values for debugging
        std::cout << "[LoadR16FTextureData] First 10 pixel values after loading:\n";
        for (int i = 0; i < 10; ++i) {
            std::cout << "Pixel[" << i << "]: Raw=" << halfFloatData[i] << "\n";
        }

        stbi_image_free(imageData);

        // Assign values to TextureData
        textureData.m_width = width;
        textureData.m_height = height;
        textureData.m_channelCount = 1;
        textureData.m_internalFormat = GL_R16F;
        textureData.m_format = GL_RED;
        textureData.m_dataSize = pixelCount * sizeof(uint16_t);
        textureData.m_imageDataType = ImageDataType::UNCOMPRESSED;
        textureData.m_data = new uint16_t[pixelCount];  // Allocate persistent memory
        std::memcpy(textureData.m_data, halfFloatData.data(), textureData.m_dataSize);

        std::cout << "[LoadR16FTextureData] Loaded R16F texture: " << filepath
            << " (" << width << "x" << height << ")\n";

        return textureData;
    }

    void SaveBitmap(const char* filename, unsigned char* data, int width, int height, int numChannels) {
        unsigned char* flippedData = (unsigned char*)malloc(width * height * numChannels);
        if (!flippedData) {
            std::cerr << "[ERROR] Failed to allocate memory for flipped data\n";
            return;
        }
        for (int y = 0; y < height; ++y) {
            std::memcpy(flippedData + (height - y - 1) * width * numChannels,
                data + y * width * numChannels,
                width * numChannels);
        }
        if (stbi_write_bmp(filename, width, height, numChannels, flippedData)) {
            std::cout << "Saved bitmap: " << filename << "\n";
        }
        else {
            std::cerr << "Failed to save bitmap: " << filename << "\n";
        }
        free(flippedData);
    }

    // Possibly broken!!!
    // Possibly broken!!!
    // Possibly broken!!!
    // Possibly broken!!!
    // Possibly broken!!!
    void SaveTextureAsBitmap(const std::vector<std::vector<uint16_t>>& pixels, int width, int height, const std::string& filename) {
        std::vector<uint8_t> imageData(width * height);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                imageData[y * width + x] = static_cast<uint8_t>(pixels[x][y] & 0xFF); // Clamp to 8-bit gray scale
            }
        }

        // Save as BMP (1-channel grayscale)
        if (stbi_write_bmp(filename.c_str(), width, height, 1, imageData.data())) {
            std::cout << "Saved " << filename << " successfully!\n";
        }
        else {
            std::cout << "Failed to save " << filename << "\n";
        }
    }

  

    void SaveBitmap(const std::string& filename, void* data, int width, int height, int format) {
        std::vector<float> floatData;

        // Infer channel count from OpenGL format
        std::unordered_map<GLenum, int> formatChannelMap = {
            {GL_R8, 1}, {GL_RG8, 2}, {GL_RGB8, 3}, {GL_RGBA8, 4},
            {GL_R16F, 1}, {GL_RG16F, 2}, {GL_RGB16F, 3}, {GL_RGBA16F, 4},
            {GL_R32F, 1}, {GL_RG32F, 2}, {GL_RGB32F, 3}, {GL_RGBA32F, 4}
        };

        if (formatChannelMap.find(format) == formatChannelMap.end()) {
            std::cout << "SaveTextureAsBitmap() failed: Unsupported format " << format << "\n";
            return;
        }
        
        int channelCount = formatChannelMap[format];
        size_t pixelCount = width * height * formatChannelMap[format];


        if (format == GL_RGBA32F || format == GL_RGB32F || format == GL_RG32F || format == GL_R32F) {
            // Data is already float, just copy it
            float* floatPtr = static_cast<float*>(data);
            floatData.assign(floatPtr, floatPtr + pixelCount);
        }
        else if (format == GL_RGBA16F || format == GL_RGB16F || format == GL_RG16F || format == GL_R16F) {
            // Convert half-floats (uint16_t) to full floats
            uint16_t* halfPtr = static_cast<uint16_t*>(data);
            floatData.resize(pixelCount);
            for (size_t i = 0; i < pixelCount; ++i) {
                floatData[i] = HalfToFloat(halfPtr[i]);
            }
        }


        // Allocate RGB buffer (BMP requires 3 channels)
        std::vector<uint8_t> outputData(width * height * 3);

        for (int i = 0; i < width * height; ++i) {
            float r, g, b;
            size_t index = i * channelCount; 

            if (channelCount == 1) {
                r = g = b = floatData[i];
            }
            else {
                r = (channelCount > 0) ? floatData[index + 0] : 0.0f;
                g = (channelCount > 1) ? floatData[index + 1] : 0.0f;
                b = (channelCount > 2) ? floatData[index + 2] : 0.0f;
            }

            // Convert to 8-bit
            outputData[i * 3 + 0] = static_cast<uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f));
            outputData[i * 3 + 1] = static_cast<uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f));
            outputData[i * 3 + 2] = static_cast<uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f));
        }

        // Save as BMP
        if (!stbi_write_bmp(filename.c_str(), width, height, 3, outputData.data())) {
            std::cout << "Error: Failed to save BMP file!\n";
        }
        else {
            std::cout << "Saved BMP successfully: " << filename << "\n";
        }
    }

    void SaveHeightMapR16F(const std::string& filename, void* data, int width, int height) {
        if (!data) {
            std::cerr << "Error: Data pointer is null, cannot save PNG.\n";
            return;
        }

        uint16_t* rawData = static_cast<uint16_t*>(data);
        size_t pixelCount = width * height;
        std::vector<uint16_t> outputData(pixelCount);

        // Find min/max values
        uint16_t minVal = 65535, maxVal = 0;
        for (size_t i = 0; i < pixelCount; ++i) {
            minVal = std::min(minVal, rawData[i]);
            maxVal = std::max(maxVal, rawData[i]);
        }

        std::cout << "[SaveHeightMapR16F] Min: " << minVal << ", Max: " << maxVal << "\n";

        // Avoid divide-by-zero when normalizing
        float range = (maxVal - minVal) > 0 ? (maxVal - minVal) : 1.0f;
        std::cout << "[SaveHeightMapR16F] Normalization range: " << range << "\n";

        // Normalize and scale to 16-bit
        for (size_t i = 0; i < pixelCount; ++i) {
            //outputData[i] = static_cast<uint16_t>(((rawData[i] - minVal) / range) * 65535.0f);
            outputData[i] = rawData[i];
        }

        // Print first 10 pixel values BEFORE saving
        std::cout << "[SaveHeightMapR16F] First 10 pixel values before saving:\n";
        for (int i = 0; i < 10; ++i) {
            std::cout << "Pixel[" << i << "]: Raw=" << rawData[i]
                << " -> Normalized=" << outputData[i] << "\n";
        }

        // Encode directly without byte swapping
        std::vector<unsigned char> png;
        unsigned error = lodepng::encode(png, reinterpret_cast<const unsigned char*>(outputData.data()), width, height, LCT_GREY, 16);

        if (error) {
            std::cerr << "[SaveHeightMapR16F] LodePNG error: " << lodepng_error_text(error) << "\n";
            return;
        }

        lodepng::save_file(png, filename);
        std::cout << "[SaveHeightMapR16F] Saved 16-bit grayscale PNG successfully: " << filename << "\n";
    }

    void SaveFloatArrayTextureAsBitmap(const std::vector<float>& data, int width, int height, int format, const std::string& filename) {
        if (data.empty()) {
            std::cout << "SaveTextureAsBitmap() failed: data was empty\n";
            return;
        }

        // Infer channel count from OpenGL format
        std::unordered_map<GLenum, int> formatChannelMap = {
            {GL_R8, 1}, {GL_RG8, 2}, {GL_RGB8, 3}, {GL_RGBA8, 4},
            {GL_R16F, 1}, {GL_RG16F, 2}, {GL_RGB16F, 3}, {GL_RGBA16F, 4},
            {GL_R32F, 1}, {GL_RG32F, 2}, {GL_RGB32F, 3}, {GL_RGBA32F, 4}
        };

        if (formatChannelMap.find(format) == formatChannelMap.end()) {
            std::cout << "SaveTextureAsBitmap() failed: Unsupported format " << format << "\n";
            return;
        }

        int channelCount = formatChannelMap[format];

        // Ensure data size matches
        size_t expectedSize = static_cast<size_t>(width) * height * channelCount;
        if (data.size() != expectedSize) {
            std::cout << "SaveTextureAsBitmap() failed: Data size mismatch. Expected " << expectedSize << ", got " << data.size() << "\n";
            return;
        }

        // Allocate RGB buffer (BMP requires 3 channels)
        std::vector<uint8_t> outputData(width * height * 3);

        for (int i = 0; i < width * height; ++i) {
            float r, g, b;

            if (channelCount == 1) { // R8 or R32F
                r = g = b = data[i];  // Convert grayscale to RGB
            }
            else {
                r = (channelCount > 0) ? data[i * channelCount + 0] : 0.0f;
                g = (channelCount > 1) ? data[i * channelCount + 1] : 0.0f;
                b = (channelCount > 2) ? data[i * channelCount + 2] : 0.0f;
            }

            // If using R8, scale from [0,255] to [0,1] before saving
            if (format == GL_R8 || format == GL_RG8) {
                r /= 255.0f;
                g /= 255.0f;
                b /= 255.0f;
            }

            // Convert to 8-bit
            outputData[i * 3 + 0] = static_cast<uint8_t>(std::clamp(r * 255.0f, 0.0f, 255.0f));
            outputData[i * 3 + 1] = static_cast<uint8_t>(std::clamp(g * 255.0f, 0.0f, 255.0f));
            outputData[i * 3 + 2] = static_cast<uint8_t>(std::clamp(b * 255.0f, 0.0f, 255.0f));
        }

        // Save as BMP
        if (!stbi_write_bmp(filename.c_str(), width, height, 3, outputData.data())) {
            std::cout << "Error: Failed to save BMP file!\n";
        }
        else {
            std::cout << "Saved BMP successfully: " << filename << "\n";
        }
    }

    void CreateFolder(const char* path) {
        std::filesystem::path dir(path);
        if (!std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir) && !std::filesystem::exists(dir)) {
                std::cout << "Failed to create directory: " << path << "\n";
            }
        }
    }

    std::string CMPFormatToString(int format) {
        switch (format) {
        case 0x0000: return "CMP_FORMAT_Unknown";
        case 0x0010: return "CMP_FORMAT_RGBA_8888_S";
        case 0x0020: return "CMP_FORMAT_ARGB_8888_S";
        case 0x0030: return "CMP_FORMAT_ARGB_8888";
        case 0x0040: return "CMP_FORMAT_ABGR_8888";
        case 0x0050: return "CMP_FORMAT_RGBA_8888";
        case 0x0060: return "CMP_FORMAT_BGRA_8888";
        case 0x0070: return "CMP_FORMAT_RGB_888";
        case 0x0080: return "CMP_FORMAT_RGB_888_S";
        case 0x0090: return "CMP_FORMAT_BGR_888";
        case 0x00A0: return "CMP_FORMAT_RG_8_S";
        case 0x00B0: return "CMP_FORMAT_RG_8";
        case 0x00C0: return "CMP_FORMAT_R_8_S";
        case 0x00D0: return "CMP_FORMAT_R_8";
        case 0x00E0: return "CMP_FORMAT_ARGB_2101010";
        case 0x00F0: return "CMP_FORMAT_RGBA_1010102";
        case 0x0100: return "CMP_FORMAT_ARGB_16";
        case 0x0110: return "CMP_FORMAT_ABGR_16";
        case 0x0120: return "CMP_FORMAT_RGBA_16";
        case 0x0130: return "CMP_FORMAT_BGRA_16";
        case 0x0140: return "CMP_FORMAT_RG_16";
        case 0x0150: return "CMP_FORMAT_R_16";
        case 0x1000: return "CMP_FORMAT_RGBE_32F";
        case 0x1010: return "CMP_FORMAT_ARGB_16F";
        case 0x1020: return "CMP_FORMAT_ABGR_16F";
        case 0x1030: return "CMP_FORMAT_RGBA_16F";
        case 0x1040: return "CMP_FORMAT_BGRA_16F";
        case 0x1050: return "CMP_FORMAT_RG_16F";
        case 0x1060: return "CMP_FORMAT_R_16F";
        case 0x1070: return "CMP_FORMAT_ARGB_32F";
        case 0x1080: return "CMP_FORMAT_ABGR_32F";
        case 0x1090: return "CMP_FORMAT_RGBA_32F";
        case 0x10A0: return "CMP_FORMAT_BGRA_32F";
        case 0x10B0: return "CMP_FORMAT_RGB_32F";
        case 0x10C0: return "CMP_FORMAT_BGR_32F";
        case 0x10D0: return "CMP_FORMAT_RG_32F";
        case 0x10E0: return "CMP_FORMAT_R_32F";
        case 0x2000: return "CMP_FORMAT_BROTLIG";
        case 0x0011: return "CMP_FORMAT_BC1";
        case 0x0021: return "CMP_FORMAT_BC2";
        case 0x0031: return "CMP_FORMAT_BC3";
        case 0x0041: return "CMP_FORMAT_BC4";
        case 0x1041: return "CMP_FORMAT_BC4_S";
        case 0x0051: return "CMP_FORMAT_BC5";
        case 0x1051: return "CMP_FORMAT_BC5_S";
        case 0x0061: return "CMP_FORMAT_BC6H";
        case 0x1061: return "CMP_FORMAT_BC6H_SF";
        case 0x0071: return "CMP_FORMAT_BC7";
        case 0x0141: return "CMP_FORMAT_ATI1N";
        case 0x0151: return "CMP_FORMAT_ATI2N";
        case 0x0152: return "CMP_FORMAT_ATI2N_XY";
        case 0x0153: return "CMP_FORMAT_ATI2N_DXT5";
        case 0x0211: return "CMP_FORMAT_DXT1";
        case 0x0221: return "CMP_FORMAT_DXT3";
        case 0x0231: return "CMP_FORMAT_DXT5";
        case 0x0252: return "CMP_FORMAT_DXT5_xGBR";
        case 0x0253: return "CMP_FORMAT_DXT5_RxBG";
        case 0x0254: return "CMP_FORMAT_DXT5_RBxG";
        case 0x0255: return "CMP_FORMAT_DXT5_xRBG";
        case 0x0256: return "CMP_FORMAT_DXT5_RGxB";
        case 0x0257: return "CMP_FORMAT_DXT5_xGxR";
        case 0x0301: return "CMP_FORMAT_ATC_RGB";
        case 0x0302: return "CMP_FORMAT_ATC_RGBA_Explicit";
        case 0x0303: return "CMP_FORMAT_ATC_RGBA_Interpolated";
        case 0x0A01: return "CMP_FORMAT_ASTC";
        case 0x0A02: return "CMP_FORMAT_APC";
        case 0x0A03: return "CMP_FORMAT_PVRTC";
        case 0x0E01: return "CMP_FORMAT_ETC_RGB";
        case 0x0E02: return "CMP_FORMAT_ETC2_RGB";
        case 0x0E03: return "CMP_FORMAT_ETC2_SRGB";
        case 0x0E04: return "CMP_FORMAT_ETC2_RGBA";
        case 0x0E05: return "CMP_FORMAT_ETC2_RGBA1";
        case 0x0E06: return "CMP_FORMAT_ETC2_SRGBA";
        case 0x0E07: return "CMP_FORMAT_ETC2_SRGBA1";
        case 0x0B01: return "CMP_FORMAT_BINARY";
        case 0x0B02: return "CMP_FORMAT_GTC";
        case 0x0B03: return "CMP_FORMAT_BASIS";
        case 0xFFFF: return "CMP_FORMAT_MAX";
        default: return "Unknown Format";
        }
    }

    std::string CMPErrorToString(int error) {
        switch (error) {
            case CMP_OK:                            return "Ok.";
            case CMP_ABORTED:                       return "The conversion was aborted.";
            case CMP_ERR_INVALID_SOURCE_TEXTURE:    return "The source texture is invalid.";
            case CMP_ERR_INVALID_DEST_TEXTURE:      return "The destination texture is invalid.";
            case CMP_ERR_UNSUPPORTED_SOURCE_FORMAT: return "The source format is not a supported format.";
            case CMP_ERR_UNSUPPORTED_DEST_FORMAT:   return "The destination format is not a supported format.";
            case CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE: return "The GPU hardware is not supported for ASTC decoding.";
            case CMP_ERR_UNSUPPORTED_GPU_BASIS_DECODE: return "The GPU hardware is not supported for BASIS decoding.";
            case CMP_ERR_SIZE_MISMATCH:             return "The source and destination texture sizes do not match.";
            case CMP_ERR_UNABLE_TO_INIT_CODEC:      return "Compressonator was unable to initialize the codec needed for conversion.";
            case CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB: return "GPU_Decode Lib was unable to initialize the codec needed for decompression.";
            case CMP_ERR_UNABLE_TO_INIT_COMPUTELIB: return "Compute Lib was unable to initialize the codec needed for compression.";
            case CMP_ERR_CMP_DESTINATION:           return "Error in compressing destination texture.";
            case CMP_ERR_MEM_ALLOC_FOR_MIPSET:      return "Memory error: allocating MIPSet compression level data buffer.";
            case CMP_ERR_UNKNOWN_DESTINATION_FORMAT: return "The destination codec type is unknown.";
            case CMP_ERR_FAILED_HOST_SETUP:         return "Failed to setup host for processing.";
            case CMP_ERR_PLUGIN_FILE_NOT_FOUND:     return "The required plugin library was not found.";
            case CMP_ERR_UNABLE_TO_LOAD_FILE:       return "The requested file was not loaded.";
            case CMP_ERR_UNABLE_TO_CREATE_ENCODER:  return "Request to create an encoder failed.";
            case CMP_ERR_UNABLE_TO_LOAD_ENCODER:    return "Unable to load an encoder library.";
            case CMP_ERR_NOSHADER_CODE_DEFINED:     return "No shader code is available for the requested framework.";
            case CMP_ERR_GPU_DOESNOT_SUPPORT_COMPUTE: return "The GPU device selected does not support compute.";
            case CMP_ERR_NOPERFSTATS:               return "No performance stats are available.";
            case CMP_ERR_GPU_DOESNOT_SUPPORT_CMP_EXT: return "The GPU does not support the requested compression extension.";
            case CMP_ERR_GAMMA_OUTOFRANGE:          return "Gamma value set for processing is out of range.";
            case CMP_ERR_PLUGIN_SHAREDIO_NOT_SET:   return "The plugin shared IO call was not set and is required for this plugin to operate.";
            case CMP_ERR_UNABLE_TO_INIT_D3DX:       return "Unable to initialize DirectX SDK or get a specific DX API.";
            case CMP_FRAMEWORK_NOT_INITIALIZED:     return "CMP_InitFramework failed or not called.";
            case CMP_ERR_GENERIC:                   return "An unknown error occurred.";
            default:                                return "Unknown CMP_ERROR value.";
        }
    }

    int GetChannelCountFromCMPFormat(int format) {
        switch (format) {
            // Uncompressed formats
            case CMP_FORMAT_RGBA_8888:
            case CMP_FORMAT_BGRA_8888:
            case CMP_FORMAT_ARGB_8888:
            case CMP_FORMAT_ABGR_8888:
            case CMP_FORMAT_RGBA_1010102:
            case CMP_FORMAT_ARGB_2101010:
                return 4; // 4 channels
            case CMP_FORMAT_RGB_888:
            case CMP_FORMAT_BGR_888:
            case CMP_FORMAT_RGBE_32F:
                return 3; // 3 channels
            case CMP_FORMAT_RG_8:
            case CMP_FORMAT_RG_16:
            case CMP_FORMAT_RG_16F:
            case CMP_FORMAT_RG_32F:
                return 2; // 2 channels
            case CMP_FORMAT_R_8:
            case CMP_FORMAT_R_16:
            case CMP_FORMAT_R_16F:
            case CMP_FORMAT_R_32F:
                return 1; // 1 channel
                // Compressed formats
            case CMP_FORMAT_BC1:    // DXT1
                return 3; // RGB
            case CMP_FORMAT_BC2:    // DXT3
            case CMP_FORMAT_BC3:    // DXT5
                return 4; // RGBA
            case CMP_FORMAT_BC4:    // ATI1
                return 1; // Red only
            case CMP_FORMAT_BC5:    // ATI2
                return 2; // Red, Green
            case CMP_FORMAT_BC6H:   // HDR (RGB Half)
                return 3; // RGB
            case CMP_FORMAT_BC7:    // RGB or RGBA
                return 4; // RGBA
            case CMP_FORMAT_ASTC:   // Adaptive Scalable Texture Compression
                return 4; // Typically RGBA
            case CMP_FORMAT_ETC_RGB:
                return 3; // RGB
            case CMP_FORMAT_ETC2_RGBA:
                return 4; // RGBA
            default:
                return 0; // Unknown or unsupported format
        }
    }

    void PrintMipSetInfo(CMP_MipSet& mipset) {
        std::cout << "Width: " << mipset.m_nWidth << "\n";
        std::cout << "Height: " << mipset.m_nHeight << "\n";
        std::cout << "Channels: " << std::to_string(mipset.m_nChannels) << "\n";
        std::cout << "Format: " << CMPFormatToString(mipset.m_format) << "\n";
        std::cout << "Mipmaps: " << mipset.m_nMaxMipLevels << "\n";
    }

   
    // Broken do not use!
    void ConvertRGBA8ToR16F(TextureData& textureData) {
        if (textureData.m_format != GL_RGBA || textureData.m_internalFormat != GL_RGBA8) {
            std::cout << "Unsupported format!\n";
        }

        std::vector<GLfloat> redChannelData;
        redChannelData.reserve(textureData.m_width * textureData.m_height);

        unsigned char* originalData = (unsigned char*)textureData.m_data;

        for (int y = 0; y < textureData.m_height; ++y) {
            for (int x = 0; x < textureData.m_width; ++x) {
                int pixelIndex = (y * textureData.m_width + x) * 4;
                unsigned char r = originalData[pixelIndex];
                redChannelData.push_back(r / 255.0f);
            }
        }

        int newDataSize = redChannelData.size() * sizeof(GLfloat);
        std::cout << "New Data Size: " << newDataSize << " bytes" << std::endl;

        textureData.m_dataSize = newDataSize;
        textureData.m_internalFormat = GL_R16F;
        textureData.m_channelCount = 1;
        textureData.m_format = GL_RED;
        if (textureData.m_data != nullptr) {
            delete[] textureData.m_data;
        }

        textureData.m_data = new GLfloat[redChannelData.size()];
        std::memcpy(textureData.m_data, redChannelData.data(), newDataSize);

        textureData.m_imageDataType = ImageDataType::UNCOMPRESSED;
    }


}