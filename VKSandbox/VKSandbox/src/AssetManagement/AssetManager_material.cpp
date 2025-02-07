#include "AssetManager.h"

namespace AssetManager {

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
        std::vector<Material>& materials = GetMaterials();
        std::vector<Texture>& textures = GetTextures();
        materials.clear();
        for (Texture& texture : textures) {
            if (FileInfoIsAlbedoTexture(texture.GetFileInfo())) {
                Material& material = materials.emplace_back(Material());
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

    Material* GetDefaultMaterial() {
        int index = GetMaterialIndex("CheckerBoard");
        return GetMaterialByIndex(index);
    }

    Material* GetMaterialByIndex(int index) {
        std::vector<Material>& materials = GetMaterials();
        if (index >= 0 && index < materials.size()) {
            Material* material = &materials[index];
            Texture* baseColor = AssetManager::GetTextureByIndex(material->m_basecolor);
            Texture* normal = AssetManager::GetTextureByIndex(material->m_normal);
            Texture* rma = AssetManager::GetTextureByIndex(material->m_rma);
            if (baseColor && baseColor->BakeComplete() &&
                normal && normal->BakeComplete() &&
                rma && rma->BakeComplete()) {
                return &materials[index];
            }
            else {
                return GetDefaultMaterial();
            }
        }
        else {
            //std::cout << "AssetManager::GetMaterialByIndex(int index) failed because index '" << index << "' is out of range. Size is " << g_materials.size() << "!\n";
            return GetDefaultMaterial();
        }
    }

    std::string GetMaterialNameByIndex(int index) {
        std::vector<Material>& materials = GetMaterials();
        if (index >= 0 && index < materials.size()) {
            return materials[index].m_name;
        }
        std::cout << "AssetManager::GetMaterialNameByIndex(int index) failed because index " << index << " was out of range of size << " << materials.size() << "\n";
        return "UNDEFINED_MATERIAL_NAME";
    }

    int GetMaterialIndex(const std::string& name) {
        std::unordered_map<std::string, int>& indexMap = GetMaterialIndexMap();
        auto it = indexMap.find(name);
        if (it != indexMap.end()) {
            return it->second;
        }
        else {
            //std::cout << "AssetManager::GetMaterialIndex(const std::string& name) failed because '" << name << "' does not exist\n";
            return -1;
        }
    }
}