#pragma once
#include "HellTypes.h"
#include "File/File.h"
#include "Renderer/Types/Mesh.hpp"
#include "Renderer/Types/Model.hpp"
#include "Renderer/Types/SkinnedModel.h"
#include "Renderer/Types/SpriteSheetTexture.h"
#include "Renderer/Types/Texture.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace AssetManager {
    void Init(); 
    void UpdateLoading();
    bool LoadingComplete();
    void AddItemToLoadLog(std::string text);
    std::vector<std::string>& GetLoadLog();

    // Animations
    std::vector<Animation>& GetAnimations();
    Animation* GetAnimationByName(const std::string& name);
    Animation* GetAnimationByIndex(int index, bool printError = true);
    int GetAnimationIndexByName(const std::string& name);

    // Materials
    std::vector<Material>& GetMaterials();
    Material* GetDefaultMaterial();
    Material* GetMaterialByIndex(int index);
    Material* GetMaterialByName(const std::string& name);
    int GetMaterialIndexByName(const std::string& name);
    std::string GetMaterialNameByIndex(int index);

    // Mesh
    std::vector<Mesh>& GetMeshes();
    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax);
    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    int GetMeshIndexByName(const std::string& name);
    int GetMeshIndexByName(const std::string& name);
    Mesh* GetMeshByName(const std::string& name);
    Mesh* GetMeshByIndex(int index);
    Mesh* GetCubeMesh();
    Mesh* GetMeshByModelNameMeshIndex(const std::string modelName, uint32_t meshIndex);

    // Models
    std::vector<Model>& GetModels();
    Model* CreateModel(const std::string& name);
    Model* GetModelByName(const std::string& name);
    Model* GetModelByIndex(int index);
    int GetModelIndexByName(const std::string& name);

    // Skinned Mesh
    std::vector<SkinnedMesh>& GetSkinnedMeshes();
    SkinnedMesh* GetSkinnedMeshByIndex(int index);
    int CreateSkinnedMesh(std::string name, std::vector<WeightedVertex>& vertices, std::vector<uint32_t>& indices, uint32_t baseVertexLocal, glm::vec3 aabbMin, glm::vec3 aabbMax);
    int GetSkinnedMeshIndexByName(const std::string& name);

    // Textures
    std::vector<Texture>& GetTextures();
    Texture* GetTextureByName(const std::string& name);
    Texture* GetTextureByIndex(int index);
    int GetTextureIndexByName(const std::string& name, bool ignoreWarning = true);
    int GetTextureCount();

    // Spritesheet Textures
    std::vector<SpriteSheetTexture>& GetSpriteSheetTextures();
    SpriteSheetTexture* GetSpriteSheetTextureByName(const std::string& textureName);
    void BuildSpriteSheetTextures();

    // Skinned Model
    std::vector<SkinnedModel>& GetSkinnedModels();
    SkinnedModel* GetSkinnedModelByName(const std::string& name);
    SkinnedModel* GetSkinnedModelByIndex(int index);
    int GetSkinnedModelIndexByName(const std::string& name);

    // Building
    void BuildHardcodedModels();
    void BuildIndexMaps();
    void BuildMaterials();

    // Loading 
    void LoadPendingTexturesAsync();
    void LoadPendingModelsAsync();
    void LoadPendingSkinnedModelsAsync();
    void LoadPendingAnimationsAsync();
    void LoadAnimation(Animation* animation);
    void LoadModel(Model* model);
    void LoadSkinnedModel(SkinnedModel* skinnedModel);
    void LoadTexture(Texture* texture);

    // Baking
    void BakePendingModels();
    void BakeModel(Model& model);

    // Import/Export
    void ExportMissingModels();
    void ExportMissingSkinnedModels();

    // Vertex data
    std::vector<Vertex>& GetVertices();
    std::vector<uint32_t>& GetIndies();
    std::vector<WeightedVertex>& GetWeightedVertices();
    std::vector<uint32_t>& GetWeightedIndies();

    // Index maps
    std::unordered_map<std::string, int>& GetTextureIndexMap();
    std::unordered_map<std::string, int>& GetMaterialIndexMap();
    std::unordered_map<std::string, int>& GetModelIndexMap();
}