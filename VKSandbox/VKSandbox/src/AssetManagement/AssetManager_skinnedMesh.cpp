#include "AssetManager.h"

namespace AssetManager {

    int g_nextWeightedVertexInsert = 0;
    int g_nextWeightedIndexInsert = 0;

    int AssetManager::CreateSkinnedMesh(const std::string& name, std::vector<WeightedVertex>& vertices, std::vector<uint32_t>& indices, uint32_t baseVertexLocal, glm::vec3 aabbMin, glm::vec3 aabbMax) {
        std::vector<SkinnedMesh>& skinnedMeshes = GetSkinnedMeshes();
        std::vector<WeightedVertex>& allVertices = GetWeightedVertices();
        std::vector<uint32_t>& allIndices = GetWeightedIndies();

        SkinnedMesh& mesh = skinnedMeshes.emplace_back();
        mesh.baseVertexGlobal = g_nextWeightedVertexInsert;
        mesh.baseVertexLocal = baseVertexLocal;
        mesh.baseIndex = g_nextWeightedIndexInsert;
        mesh.vertexCount = (uint32_t)vertices.size();
        mesh.indexCount = (uint32_t)indices.size();
        mesh.name = name;
        mesh.aabbMin = aabbMin;
        mesh.aabbMax = aabbMax;

        allVertices.reserve(allVertices.size() + vertices.size());
        allVertices.insert(std::end(allVertices), std::begin(vertices), std::end(vertices));

        allIndices.reserve(allIndices.size() + indices.size());
        allIndices.insert(std::end(allIndices), std::begin(indices), std::end(indices));

        g_nextWeightedVertexInsert += mesh.vertexCount;
        g_nextWeightedIndexInsert += mesh.indexCount;

        return skinnedMeshes.size() - 1;
    }

    SkinnedMesh* AssetManager::GetSkinnedMeshByIndex(int index) {
        std::vector<SkinnedMesh>& skinnedMeshes = GetSkinnedMeshes();
        if (index >= 0 && index < skinnedMeshes.size()) {
            return &skinnedMeshes[index];
        }
        else {
            std::cout << "AssetManager::GetSkinnedMeshByIndex() failed because index '" << index << "' is out of range. Size is " << skinnedMeshes.size() << "!\n";
            return nullptr;
        }
    }

    int AssetManager::GetSkinnedMeshIndexByName(const std::string& name) {
        std::vector<SkinnedMesh>& skinnedMeshes = GetSkinnedMeshes();
        for (int i = 0; i < skinnedMeshes.size(); i++) {
            if (skinnedMeshes[i].name == name) {
                return i;
            }
        }
        std::cout << "AssetManager::GetSkinnedMeshIndexByName() failed because name '" << name << "' was not found in _skinnedMeshes!\n";
        return -1;
    }
}