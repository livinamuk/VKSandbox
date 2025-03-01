#include "HeightMapManager.h"
#include "Backend/Backend.h"
#include "File/File.h"
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
        std::vector<FileInfo> files = Util::IterateDirectory("res/height_maps/");

        // Iterate all found files and upload the data to the gpu
        for (int i = 0; i < files.size(); i++) {
            HeightMapData heightMapData = File::LoadHeightMap(files[i].name + ".heightmap");

            if (BackEnd::GetAPI() == API::OPENGL) {
                g_glTextureArray.SetLayerDataR16(i, heightMapData.data);
            }
            else if (BackEnd::GetAPI() == API::OPENGL) {
                // TODO
            }
        }
    }

    OpenGLTextureArray& GetGLTextureArray() {
        return g_glTextureArray;
    }
}