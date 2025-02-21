#include "HeightMapManager.h"
#include "Backend/Backend.h"
#include "Tools/ImageTools.h"
#include "Util.h"
#include <glad/glad.h>

namespace HeightMapManager {

    OpenGLTextureArray g_glTextureArray;
    uint32_t g_heightmapCount = 0;

    void AllocateMemory(int heightmapCount);
    void LoadHeightMapsFromDisk();

    void Init() {
        AllocateMemory(16);
        LoadHeightMapsFromDisk();
    }

    void AllocateMemory(int heightmapCount) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            g_glTextureArray.AllocateMemory(HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE, GL_R16F, 1, heightmapCount);
        }
        else if (BackEnd::GetAPI() == API::OPENGL) {
            // TODO
        }
    }

    

    void LoadHeightMapsFromDisk() {
        std::cout << "\n";
        for (FileInfo& fileInfo : Util::IterateDirectory("res/height_maps/256x256")) {
            //TextureData textureData = ImageTools::LoadR16FTextureData(fileInfo.path);
            TextureData textureData = ImageTools::LoadUncompressedTextureData(fileInfo.path);
            Util::PrintDebugInfo(textureData);

            ImageTools::ConvertRGBA8ToR16F(textureData);

            if (BackEnd::GetAPI() == API::OPENGL) {
                g_glTextureArray.LoadTextureData(textureData, g_heightmapCount++);
            }
            else if (BackEnd::GetAPI() == API::OPENGL) {
                // TODO
            }

            std::cout << "Loaded " << fileInfo.name << "\n";
            Util::PrintDebugInfo(textureData);
            std::cout << "\n";
        }
    }

    OpenGLTextureArray& GetGLTextureArray() {
        return g_glTextureArray;
    }
}