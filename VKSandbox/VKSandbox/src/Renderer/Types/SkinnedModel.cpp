#include "SkinnedModel.h"
#include "AssetManagement/AssetManager.h"
#include "Util/Util.h"

void SkinnedModel::Load(SkinnedModelData& skinnedModelData) {
    this->m_vertexCount = skinnedModelData.vertexCount;
    this->m_indexCount = skinnedModelData.indexCount;
    this->m_boneOffsets = skinnedModelData.boneOffsets;
    this->m_nodes = skinnedModelData.nodes;
    this->m_boneMapping = skinnedModelData.boneMapping;

    for (SkinnedMeshData& skinnedMeshData : skinnedModelData.meshes) {
        const std::string& meshName = skinnedMeshData.name;
        const glm::vec3 aabbMin = skinnedMeshData.aabbMin;
        const glm::vec3 aabbMax = skinnedMeshData.aabbMax;
        const uint32_t baseVertexLocal = skinnedMeshData.localBaseVertex;
        std::vector<WeightedVertex>& vertices = skinnedMeshData.vertices;
        std::vector<uint32_t>& indices = skinnedMeshData.indices;

        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        this->AddMeshIndex(AssetManager::CreateSkinnedMesh(meshName, vertices, indices, baseVertexLocal, aabbMin, aabbMax));
    }
}

void SkinnedModel::SetFileInfo(FileInfo fileInfo) {
    m_fileInfo = fileInfo;
}

const FileInfo& SkinnedModel::GetFileInfo() {
    return m_fileInfo;
}

LoadingState& SkinnedModel::GetLoadingState() {
    return m_loadingState;
}

void SkinnedModel::SetVertexCount(uint32_t vertexCount) {
    m_vertexCount = vertexCount;
}

void SkinnedModel::SetLoadingState(LoadingState loadingState) {
    m_loadingState = loadingState;
}

const std::string& SkinnedModel::GetName() {
    return m_fileInfo.name;
}

void SkinnedModel::AddMeshIndex(uint32_t index) {
    m_meshIndices.push_back(index);
}

uint32_t SkinnedModel::GetMeshCount() {
    return m_meshIndices.size();
}

std::vector<uint32_t>& SkinnedModel::GetMeshIndices() {
    return m_meshIndices;
}

uint32_t SkinnedModel::GetVertexCount() { 
    return m_vertexCount; 
}

uint32_t SkinnedModel::GetBoneCount() { 
    return m_boneMapping.size(); 
}