#pragma once
#include "AssetManager.h"
#include <thread>
#include "../AssetManagement/BakeQueue.h"
#include "../API/OpenGL/GL_backend.h"
#include "../API/Vulkan/VK_backend.h"
#include "../BackEnd/Backend.h"
#include "../File/AssimpImporter.h"
#include "../Tools/ImageTools.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Types/Mesh.hpp"
#include "../UI/UIBackEnd.h"
#include "../Util.hpp"

namespace AssetManager {

    std::vector<Texture> g_textures;
    std::vector<Material> g_materials;
    std::vector<Model> g_models;
    std::vector<Mesh> g_meshes;
    std::unordered_map<std::string, int> g_textureIndexMap;
    std::unordered_map<std::string, int> g_materialIndexMap;
    std::unordered_map<std::string, int> g_modelIndexMap;
    std::vector<std::string> g_loadLog;
    std::vector<std::future<void>> g_futures;
    bool g_loadingComplete = false;

    std::vector<Vertex> g_vertices;
    std::vector<uint32_t> g_indices;
    int g_nextVertexInsert = 0;
    int g_nextIndexInsert = 0;

    void AddItemToLoadLog(std::string text);
    void CompressMissingDDSTexutres();
    void ExportMissingModels();
    void FindAssetPaths();
    void LoadMinimumTextures();
    void LoadModel(Model* model);
    void LoadTexture(Texture* texture);
    void BakeModel(Model& model);
    void BuildIndexMaps();
    void BuildMaterials();
    void LoadPendingTexturesAsync();
    void LoadPendingModelsAsync();
    void BakePendingModels();

    bool FileInfoIsAlbedoTexture(const FileInfo& fileInfo);
    std::string GetMaterialNameFromFileInfo(const FileInfo& fileInfo);

    void Init() {
        CompressMissingDDSTexutres();
        ExportMissingModels();
        LoadMinimumTextures();
        FindAssetPaths();

        // Fire off all async loading calls
        LoadPendingTexturesAsync();
        LoadPendingModelsAsync();
    }

    bool LoadingComplete() {
        return g_loadingComplete;
    }

    void UpdateLoading() {

        // Calculate load log text
        std::string text = "";
        int maxLinesDisplayed = 36;
        int endIndex = AssetManager::GetLoadLog().size();
        int beginIndex = std::max(0, endIndex - maxLinesDisplayed);
        for (int i = beginIndex; i < endIndex; i++) {
            text += AssetManager::GetLoadLog()[i] + "\n";
        }

        UIBackEnd::BlitText(text, "StandardFont", 0, 0, 2.0f);

        // Loading complete?
        g_loadingComplete = true;
        for (Texture& texture : g_textures) {
            texture.CheckForBakeCompletion();
            if (!texture.BakeComplete()) {
                g_loadingComplete = false;
                return;
            }
        }
        for (Model& model : g_models) {
            if (model.GetLoadingState() != LoadingState::LOADING_COMPLETE) {
                g_loadingComplete = false;
                std::cout << model.GetFileInfo().name;
                return;
            }
        }

        if (LoadingComplete()) {
            BakePendingModels();
            BuildMaterials();
            BuildIndexMaps();
            if (BackEnd::GetAPI() == API::OPENGL) {
                OpenGLBackEnd::UploadVertexData(g_vertices, g_indices);
            }
            Renderer::InitMain();
            std::cout << "Assets loaded\n";
        }
    }

    void ExportMissingModels() {
        // Scan for new obj and fbx and export custom model format
        for (FileInfo& fileInfo : Util::IterateDirectory("res/models_raw", { "obj", "fbx" })) {
            std::string assetPath = "res/models/" + fileInfo.name + ".model";

            // If the file exists but timestamps don't match, re-export
            if (Util::FileExists(assetPath)) {
                uint64_t lastModified = File::GetLastModifiedTime(fileInfo.path);
                ModelHeader modelHeader = File::ReadModelHeader(assetPath);
                if (modelHeader.timestamp != lastModified) {
                    File::DeleteFile(assetPath);
                    ModelData modelData = AssimpImporter::ImportFbx(fileInfo.path);
                    File::ExportModel(modelData);
                }
            }
            // File doesn't even exist yet, so export it
            else {
                ModelData modelData = AssimpImporter::ImportFbx(fileInfo.path);
                File::ExportModel(modelData);
            }
        }
    }

    void FindAssetPaths() {
        // Find .model files
        for (FileInfo& fileInfo : Util::IterateDirectory("res/models")) {
            Model& model = g_models.emplace_back();
            model.SetFileInfo(fileInfo);
        }
        // Find all textures
        for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/uncompressed", { "png", "jpg", "tga" })) {
            Texture& texture = g_textures.emplace_back();
            texture.SetFileInfo(fileInfo);
            texture.SetImageDataType(ImageDataType::UNCOMPRESSED);
            texture.SetTextureWrapMode(TextureWrapMode::REPEAT);
            texture.SetMinFilter(TextureFilter::LINEAR_MIPMAP);
            texture.SetMagFilter(TextureFilter::LINEAR);
            texture.RequestMipmaps();
        }
        for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/compressed", { "dds" })) {
            Texture& texture = g_textures.emplace_back();
            texture.SetFileInfo(fileInfo);
            texture.SetImageDataType(ImageDataType::COMPRESSED);
            texture.SetTextureWrapMode(TextureWrapMode::REPEAT);
            texture.SetMinFilter(TextureFilter::LINEAR_MIPMAP);
            texture.SetMagFilter(TextureFilter::LINEAR);
            texture.RequestMipmaps();
        }
        for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/ui", { "png", "jpg", })) {
            Texture& texture = g_textures.emplace_back();
            texture.SetFileInfo(fileInfo);
            texture.SetImageDataType(ImageDataType::UNCOMPRESSED);
            texture.SetTextureWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
            texture.SetMinFilter(TextureFilter::LINEAR);
            texture.SetMagFilter(TextureFilter::LINEAR);
        }
    }

    void LoadPendingModelsAsync() {
        for (Model& model : g_models) {
            if (model.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
                model.SetLoadingState(LoadingState::LOADING_FROM_DISK);
                AddItemToLoadLog(model.GetFileInfo().path);
                g_futures.push_back(std::async(std::launch::async, LoadModel, &model));
            }
        }
    }

    void LoadModel(Model* model) {
        model->m_modelData = File::ImportModel("res/models/" + model->GetFileInfo().GetFileNameWithExtension());
        model->SetLoadingState(LoadingState::LOADING_COMPLETE);
    }


    void BakePendingModels() {
        for (Model& model : g_models) {
            if (model.GetLoadingState() == LoadingState::LOADING_COMPLETE) {
                BakeModel(model);
                AddItemToLoadLog(model.GetFileInfo().path);
            }
        }
    }

    void BakeModel(Model& model) {
        model.SetName(model.m_modelData.name);
        model.SetAABB(model.m_modelData.aabbMin, model.m_modelData.aabbMax);
        for (MeshData& meshData : model.m_modelData.meshes) {
            int meshIndex = CreateMesh(meshData.name, meshData.vertices, meshData.indices, meshData.aabbMin, meshData.aabbMax);
            model.AddMeshIndex(meshIndex);
        }
    }


    void LoadMinimumTextures() {
        // Find files
        for (FileInfo& fileInfo : Util::IterateDirectory("res/fonts", { "png" })) {
            Texture& texture = g_textures.emplace_back();
            texture.SetFileInfo(fileInfo);
            texture.SetImageDataType(ImageDataType::UNCOMPRESSED);
            texture.SetTextureWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
            texture.SetMinFilter(TextureFilter::NEAREST);
            texture.SetMagFilter(TextureFilter::NEAREST);
        }
        LoadPendingTexturesAsync();
        BakeQueue::ImmediateBakeAllTextures();
        BuildIndexMaps();
    }

    void AddItemToLoadLog(std::string text) {
        std::replace(text.begin(), text.end(), '\\', '/');
        g_loadLog.push_back(text);
        //static std::mutex mutex;
        //std::lock_guard<std::mutex> lock(mutex);
        //std::cout << text << "\n";
    }

    std::vector<std::string>& GetLoadLog() {
        return g_loadLog;
    }

    void BuildIndexMaps() {
        g_modelIndexMap.clear();
        g_textureIndexMap.clear();
        g_materialIndexMap.clear();
        for (int i = 0; i < g_models.size(); i++) {
            g_modelIndexMap[g_models[i].GetName()] = i;
        }
        for (int i = 0; i < g_textures.size(); i++) {
            g_textureIndexMap[g_textures[i].GetFileInfo().name] = i;
        }
        for (int i = 0; i < g_materials.size(); i++) {
            g_materialIndexMap[g_materials[i].m_name] = i;
        }
    }

    /*
    █▄ ▄█ █▀█ █▀▄ █▀▀ █
    █ █ █ █ █ █ █ █▀▀ █
    ▀   ▀ ▀▀▀ ▀▀  ▀▀▀ ▀▀▀ */

    // ?

    /*
    █▄ ▄█ █▀▀ █▀▀ █ █
    █ █ █ █▀▀ ▀▀█ █▀█
    ▀   ▀ ▀▀▀ ▀▀▀ ▀ ▀ */

    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax) {
        Mesh& mesh = g_meshes.emplace_back();
        mesh.baseVertex = g_nextVertexInsert;
        mesh.baseIndex = g_nextIndexInsert;
        mesh.vertexCount = (uint32_t)vertices.size();
        mesh.indexCount = (uint32_t)indices.size();
        mesh.SetName(name);
        mesh.aabbMin = aabbMin;
        mesh.aabbMax = aabbMax;
        mesh.extents = aabbMax - aabbMin;
        mesh.boundingSphereRadius = std::max(mesh.extents.x, std::max(mesh.extents.y, mesh.extents.z)) * 0.5f;

        g_vertices.reserve(g_vertices.size() + vertices.size());
        g_vertices.insert(std::end(g_vertices), std::begin(vertices), std::end(vertices));
        g_indices.reserve(g_indices.size() + indices.size());
        g_indices.insert(std::end(g_indices), std::begin(indices), std::end(indices));
        g_nextVertexInsert += mesh.vertexCount;
        g_nextIndexInsert += mesh.indexCount;
        return g_meshes.size() - 1;
    }

    int GetMeshIndexByName(const std::string& name) {
        for (int i = 0; i < g_meshes.size(); i++) {
            if (g_meshes[i].GetName() == name)
                return i;
        }
        std::cout << "AssetManager::GetMeshIndexByName() failed because '" << name << "' does not exist\n";
        return -1;
    }

    Mesh* GetMeshByName(const std::string& name) {
        for (int i = 0; i < g_meshes.size(); i++) {
            if (g_meshes[i].GetName() == name)
                return &g_meshes[i];
        }
        std::cout << "AssetManager::GetMeshByName() failed because '" << name << "' does not exist\n";
        return nullptr;
    }

    Mesh* GetMeshByIndex(int index) {
        if (index >= 0 && index < g_meshes.size()) {
            return &g_meshes[index];
        }
        else {
            std::cout << "AssetManager::GetMeshByIndex() failed because index '" << index << "' is out of range. Size is " << g_meshes.size() << "!\n";
            return nullptr;
        }
    }

    /*
     ▀█▀ █▀▀ █ █ ▀█▀ █ █ █▀▄ █▀▀
      █  █▀▀ ▄▀▄  █  █ █ █▀▄ █▀▀
      ▀  ▀▀▀ ▀ ▀  ▀  ▀▀▀ ▀ ▀ ▀▀▀ */

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
        std::vector<std::future<void>> futures;
        for (Texture& texture : g_textures) {
            if (texture.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
                texture.SetLoadingState(LoadingState::LOADING_FROM_DISK);
                futures.emplace_back(std::async(std::launch::async, LoadTexture, &texture));
            }
        }
        for (auto& future : futures) {
            future.get();
        }
        // Allocate gpu memory
        for (Texture& texture : g_textures) {
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
        for (Texture& texture : g_textures) {
            if (texture.GetFileInfo().name == name)
                return &texture;
        }
        std::cout << "AssetManager::GetTextureByName() failed because '" << name << "' does not exist\n";
        return nullptr;
    }

    int GetTextureIndexByName(const std::string& name, bool ignoreWarning) {
        for (int i = 0; i < g_textures.size(); i++) {
            if (g_textures[i].GetFileInfo().name == name)
                return i;
        }
        if (!ignoreWarning) {
            std::cout << "AssetManager::GetTextureIndexByName() failed because '" << name << "' does not exist\n";
        }
        return -1;
    }

    Texture* GetTextureByIndex(int index) {
        if (index != -1) {
            return &g_textures[index];
        }
        std::cout << "AssetManager::GetTextureByIndex() failed because index was -1\n";
        return nullptr;
    }

    int GetTextureCount() {
        return g_textures.size();
    }

    /*
    █▄ ▄█ █▀█ ▀█▀ █▀▀ █▀▄ ▀█▀ █▀█ █
    █ █ █ █▀█  █  █▀▀ █▀▄  █  █▀█ █
    ▀   ▀ ▀ ▀  ▀  ▀▀▀ ▀ ▀ ▀▀▀ ▀ ▀ ▀▀▀ */

    bool FileInfoIsAlbedoTexture(const FileInfo& fileInfo) {
        if (fileInfo.name.size() >= 4 && fileInfo.name.substr(fileInfo.name.size() - 4) == "_ALB") {
            return true;
        }
        return false;
    }

    std::string GetMaterialNameFromFileInfo(const FileInfo& fileInfo) {
        const std::string suffix = "_ALB";
        if (fileInfo.name.size() > suffix.size() && fileInfo.name.substr(fileInfo.name.size() - suffix.size()) == suffix) {
            return fileInfo.name.substr(0, fileInfo.name.size() - suffix.size());
        }
        return "";
    }

    void BuildMaterials() {
        g_materials.clear();
        for (Texture& texture : g_textures) {
            if (FileInfoIsAlbedoTexture(texture.GetFileInfo())) {
                Material& material = g_materials.emplace_back(Material());
                material.m_name = GetMaterialNameFromFileInfo(texture.GetFileInfo());
                int basecolorIndex = GetTextureIndexByName(material.m_name + "_ALB", true);
                int normalIndex = GetTextureIndexByName(material.m_name + "_NRM", true);
                int rmaIndex = GetTextureIndexByName(material.m_name + "_RMA", true);
                material.m_basecolor = basecolorIndex;
                material.m_normal = (normalIndex != -1) ? normalIndex : GetTextureIndexByName("DefaultNRM");
                material.m_rma = (rmaIndex != -1) ? rmaIndex : GetTextureIndexByName("DefaultRMA");
            }
        }
    }

    

    Model* GetModelByIndex(int index) {
        if (index != -1) {
            return &g_models[index];
        }
        std::cout << "AssetManager::GetModelByIndex() failed because index was -1\n";
        return nullptr;
    }

    Model* CreateModel(const std::string& name) {
        g_models.emplace_back();
        Model* model = &g_models[g_models.size() - 1];
        model->SetName(name);
        return model;
    }

    int GetModelIndexByName(const std::string& name) {
        auto it = g_modelIndexMap.find(name);
        if (it != g_modelIndexMap.end()) {
            return it->second;
        }
        std::cout << "AssetManager::GetModelIndexByName() failed because name '" << name << "' was not found in _models!\n";
        return -1;
    }

    Material* GetDefaultMaterial() {
        int index = GetMaterialIndex("CheckerBoard");
        return GetMaterialByIndex(index);
    }

    Material* GetMaterialByIndex(int index) {
        if (index >= 0 && index < g_materials.size()) {
            Material* material = &g_materials[index];
            Texture* baseColor = AssetManager::GetTextureByIndex(material->m_basecolor);
            Texture* normal = AssetManager::GetTextureByIndex(material->m_normal);
            Texture* rma = AssetManager::GetTextureByIndex(material->m_rma);
            if (baseColor && baseColor->BakeComplete() && 
                normal && normal->BakeComplete() && 
                rma && rma->BakeComplete()) {
                return &g_materials[index]; 
            }
            else {
                return GetDefaultMaterial();
            }
        }
        else {
            //std::cout << "AssetManager::GetMaterialByIndex() failed because index '" << index << "' is out of range. Size is " << g_materials.size() << "!\n";
            return GetDefaultMaterial();
        }
    }

    int GetMaterialIndex(const std::string& name) {
        auto it = g_materialIndexMap.find(name);
        if (it != g_materialIndexMap.end()) {
            return it->second;
        }
        else {
            //std::cout << "AssetManager::GetMaterialIndex() failed because '" << name << "' does not exist\n";
            return -1;
        }
    }

    std::string& GetMaterialNameByIndex(int index) {
        return g_materials[index].m_name;
    }
}




    



    

