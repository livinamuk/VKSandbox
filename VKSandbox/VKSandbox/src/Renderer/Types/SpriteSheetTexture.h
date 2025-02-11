#pragma once
#include "HellTypes.h"`
#include <string>

struct SpriteSheetTexture {
    SpriteSheetTexture() = default;
    void Init();
    void SetFileInfo(FileInfo fileInfo);
    
    std::string GetDebugInfo();
    FileInfo GetFileInfo();

private:
    FileInfo m_fileInfo;
    std::string m_textureName;    
    uint32_t m_rows = 0;
    uint32_t m_columns = 0;
    int32_t m_fullWidth = 0;
    int32_t m_fullHeight = 0;
    int32_t m_channelCount = 0;
    uint32_t m_frameWidth = 0;
    uint32_t m_frameHeight = 0;
    uint32_t m_frameCount = 0;
};