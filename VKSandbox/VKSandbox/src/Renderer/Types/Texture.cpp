#include "Texture.h"
#include "../API/OpenGL/GL_Util.h"
#include "../AssetManagement/AssetManager.h"
#include "../Tools/ImageTools.h"
#include "../Util/Util.h"
#include "stb_image.h"

void Texture::Load() {
    // Load texture data from disk
    if (m_imageDataType == ImageDataType::UNCOMPRESSED) {
        m_textureDataLevels = { ImageTools::LoadUncompressedTextureData(m_fileInfo.path) };
    }
    else if (m_imageDataType == ImageDataType::COMPRESSED) {
        m_textureDataLevels = ImageTools::LoadTextureDataFromDDSThreadSafe(m_fileInfo.path);
    }
    else if (m_imageDataType == ImageDataType::EXR) {
        m_textureDataLevels = { ImageTools::LoadEXRData(m_fileInfo.path) };
    }
    m_loadingState = LoadingState::LOADING_COMPLETE;

    // Calculate mipmap level count
    m_mipmapLevelCount = 1 + static_cast<int>(std::log2(std::max(GetWidth(0), GetHeight(0))));

    // Initiate bake states
    m_textureDataLevelBakeStates.resize(m_textureDataLevels.size(), BakeState::AWAITING_BAKE);
}

void Texture::FreeCPUMemory() {
    for (TextureData& textureData : m_textureDataLevels) {
        stbi_image_free(textureData.m_data);
        textureData.m_data = nullptr;
    }
}

const int Texture::GetWidth(int mipmapLevel) {
    if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
        return m_textureDataLevels[mipmapLevel].m_width;
    }
    else {
        std::cout << "Texture::GetWidth(int mipmapLevel) failed. mipmapLevel '" << mipmapLevel << "' out of range of size " << m_textureDataLevels.size() << "\n";
        return 0;
    }
}

const int Texture::GetHeight(int mipmapLevel) {
    if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
        return m_textureDataLevels[mipmapLevel].m_height;
    }
    else {
        std::cout << "Texture::GetHeight(int mipmapLevel) failed. mipmapLevel '" << mipmapLevel << "' out of range of size " << m_textureDataLevels.size() << "\n";
        return 0;
    }
}

const void* Texture::GetData(int mipmapLevel) {
    if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
        return m_textureDataLevels[mipmapLevel].m_data;
    }
    else {
        std::cout << "Texture::GetData(int mipmapLevel) failed. mipmapLevel '" << mipmapLevel << "' out of range of size " << m_textureDataLevels.size() << "\n";
        return nullptr;
    }
}

const int Texture::GetDataSize(int mipmapLevel) {
    if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
        return m_textureDataLevels[mipmapLevel].m_dataSize;
    }
    else {
        std::cout << "Texture::GetDataSize(int mipmapLevel) failed. mipmapLevel '" << mipmapLevel << "' out of range of size " << m_textureDataLevels.size() << "\n";
        return 0;
    }
}

const int Texture::GetFormat() {
    if (!m_textureDataLevels.empty()) {
        return m_textureDataLevels[0].m_format;
    }
    else {
        std::cout << "Texture::GetFormat() failed. m_textureData is empty\n";
        return 0;
    }
}

const int Texture::GetChannelCount() {
    if (!m_textureDataLevels.empty()) {
        return m_textureDataLevels[0].m_channelCount;
    }
    else {
        std::cout << "Texture::GetChannelCount() failed. m_textureData is empty\n";
        return 0;
    }
}
const int Texture::GetInternalFormat() {
    if (!m_textureDataLevels.empty()) {
        return m_textureDataLevels[0].m_internalFormat;
    }
    else {
        std::cout << "Texture::GetInternalFormat() failed. m_textureData is empty\n";
        return 0;
    }
}

const int Texture::GetMipmapLevelCount() {
    return m_mipmapLevelCount;
}

const std::string& Texture::GetFileName() {
    return m_fileInfo.name;
}

const std::string& Texture::GetFilePath() {
    return m_fileInfo.path;
}

OpenGLTexture& Texture::GetGLTexture() {
    return m_glTexture;
}

VulkanTexture& Texture::GetVKTexture() {
    return m_vkTexture;
}

void Texture::SetLoadingState(LoadingState state) {
    m_loadingState = state;
}

void Texture::SetTextureDataLevelBakeState(int index, BakeState state) {
    if (index >= 0 && m_textureDataLevelBakeStates.size() && index < m_textureDataLevelBakeStates.size()) {
        m_textureDataLevelBakeStates[index] = state;
    }
    else {
        std::cout << "Texture::SetTextureDataLevelBakeState(int index, BakeState state) failed. Index '" << index << "' out of range of size " << m_textureDataLevelBakeStates.size() << "\n";
    }
}

void Texture::SetFileInfo(FileInfo fileInfo) {
    m_fileInfo = fileInfo;
}

void Texture::SetImageDataType(ImageDataType imageDataType) {
    m_imageDataType = imageDataType;
}

void Texture::SetTextureWrapMode(TextureWrapMode wrapMode) {
    m_wrapMode = wrapMode;
}

void Texture::SetMinFilter(TextureFilter filter) {
    m_minFilter = filter;
}

void Texture::SetMagFilter(TextureFilter filter) {
    m_magFilter = filter;
}

const LoadingState Texture::GetLoadingState() {
    return m_loadingState;
}

const BakeState Texture::GetTextureDataLevelBakeState(int index) {
    if (index >= 0 && m_textureDataLevelBakeStates.size() && index < m_textureDataLevelBakeStates.size()) {
        return m_textureDataLevelBakeStates[index];
    }
    else {
        std::cout << "Texture::GetTextureDataLevelBakeState(int index) failed. Index '" << index << "' out of range of size " << m_textureDataLevelBakeStates.size() << "\n";
        return BakeState::UNDEFINED;
    }
}

const FileInfo Texture::GetFileInfo() {
    return m_fileInfo;
}

const ImageDataType Texture::GetImageDataType() {
    return m_imageDataType;
}
const TextureWrapMode Texture::GetTextureWrapMode() {
    return m_wrapMode;
}

const TextureFilter Texture::GetMinFilter() {
    return m_minFilter;
}

const TextureFilter Texture::GetMagFilter() {
    return m_magFilter;
}

void Texture::CheckForBakeCompletion() {
    if (m_bakeComplete) {
        return;
    }
    else {
        m_bakeComplete = true;
        for (BakeState& state : m_textureDataLevelBakeStates) {
            if (state != BakeState::BAKE_COMPLETE) {
                m_bakeComplete = false;
                return;
            }
        }
        // Bake is complete!
        AssetManager::AddItemToLoadLog(GetFilePath());
    }
}

const bool Texture::BakeComplete() {
    return m_bakeComplete;
}

const int Texture::GetTextureDataCount() {
    return m_textureDataLevels.size();
}

void Texture::RequestMipmaps() {
    m_mipmapsRequested = true;
}

const bool Texture::MipmapsAreRequested() {
    return m_mipmapsRequested;
}

const void Texture::PrintDebugInfo() {
    std::cout << GetFileName() << "\n";
    std::cout << " - width: " << GetWidth(0) << "\n";
    std::cout << " - height: " << GetHeight(0) << "\n";
    std::cout << " - channel count: " << GetChannelCount() << "\n";
    std::cout << " - internal format: " << OpenGLUtil::GLInternalFormatToString(GetInternalFormat()) << "\n";
    std::cout << " - format: " << OpenGLUtil::GLFormatToString(GetFormat()) << "\n";
    std::cout << " - mipmap level count: " << GetMipmapLevelCount() << "\n";
    std::cout << " - mipmaps requested: " << (MipmapsAreRequested() ? "TRUE" : "FALSE") << "\n";
    std::cout << " - data size:\n";
    for (int i = 0; i < GetTextureDataCount(); i++) {
        std::cout << "   mip " << i << " " << GetDataSize(i) << " at " << GetData(i) << "\n";
    }
    std::cout << "\n";
}