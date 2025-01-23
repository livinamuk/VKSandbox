#pragma once
#include "HellTypes.h"
#include <string>
#include "../File/File.h"
#include "../Types/DetachedMesh.hpp"
#include "../Types/Model.hpp"
#include "../Types/Texture.h"

namespace AssetManager {
    void Init(); 
    void UpdateLoading();
    bool LoadingComplete();
    void AddItemToLoadLog(std::string text);
    std::vector<std::string>& GetLoadLog();

    // Textures
    int GetTextureIndexByName(const std::string& name, bool ignoreWarning = true);
    int GetTextureCount();
    Texture* GetTextureByName(const std::string& name);
    Texture* GetTextureByIndex(int index);

    // Models
    int GetModelIndexByName(const std::string& name);
    Model* CreateModel(const std::string& name);
    Model* GetModelByIndex(int index);

    // Mesh
    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax);
    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    int GetMeshIndexByName(const std::string& name);
    int GetMeshIndexByName(const std::string& name);
    DetachedMesh* GetMeshByName(const std::string& name);
    DetachedMesh* GetMeshByIndex(int index);

    // Materials
    Material* GetDefaultMaterial();
    Material* GetMaterialByIndex(int index);
    int GetMaterialIndex(const std::string& name);
    std::string& GetMaterialNameByIndex(int index);
}