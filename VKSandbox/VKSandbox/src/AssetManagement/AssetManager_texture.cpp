#include "AssetManager.h"
#include "BakeQueue.h"
#include "API/OpenGL/GL_backend.h"
#include "API/Vulkan/VK_backend.h"
#include "BackEnd/BackEnd.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"
#include <future>

namespace AssetManager {

    void CompressMissingDDSTexutres() {
        for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/compress_me", { "png", "jpg", "tga" })) {
            std::string inputPath = fileInfo.path;
            std::string outputPath = "res/textures/compressed/" + fileInfo.name + ".dds";
            if (!Util::FileExists(outputPath)) {
                ImageTools::CreateAndExportDDS(inputPath, outputPath, true);
                std::cout << "Exported " << outputPath << "\n";
            }
        }
    }

    void LoadPendingTexturesAsync() {
        std::vector<Texture>& textures = GetTextures();
        std::vector<std::future<void>> futures;

        for (Texture& texture : textures) {
            if (texture.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
                texture.SetLoadingState(LoadingState::LOADING_FROM_DISK);
                futures.emplace_back(std::async(std::launch::async, LoadTexture, &texture));
            }
        }
        for (auto& future : futures) {
            future.get();
        }
        // Allocate gpu memory
        for (Texture& texture : textures) {
            if (BackEnd::GetAPI() == API::OPENGL) {
                OpenGLBackEnd::AllocateTextureMemory(texture);
            }
            else if (BackEnd::GetAPI() == API::VULKAN) {
                VulkanBackEnd::AllocateTextureMemory(texture);
            }
        }
    }

    void LoadTexture(Texture* texture) {
        if (texture) {
            texture->Load();
            BakeQueue::QueueTextureForBaking(texture);
        }
    }

    Texture* GetTextureByName(const std::string& name) {
        std::vector<Texture>& textures = GetTextures();
        for (Texture& texture : textures) {
            if (texture.GetFileInfo().name == name)
                return &texture;
        }
        std::cout << "AssetManager::GetTextureByName(const std::string& name) failed because '" << name << "' does not exist\n";
        return nullptr;
    }

    int GetTextureIndexByName(const std::string& name, bool ignoreWarning) {
        std::unordered_map<std::string, int>& indexMap = GetTextureIndexMap();
        auto it = indexMap.find(name);
        if (it != indexMap.end()) {
            return it->second;
        }
        if (!ignoreWarning) {
            std::cout << "AssetManager::GetTextureIndexByName(const std::string& name) failed because name '" << name << "' was not found in g_textures\n";
        }
        return -1;
    }

    Texture* GetTextureByIndex(int index) {
        std::vector<Texture>& textures = GetTextures();
        if (index != -1) {
            return &textures[index];
        }
        std::cout << "AssetManager::GetTextureByIndex() failed because index was -1\n";
        return nullptr;
    }

    int GetTextureCount() {
        return GetTextures().size();
    }
}