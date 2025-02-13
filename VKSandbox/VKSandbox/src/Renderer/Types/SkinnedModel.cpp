#include "SkinnedModel.h"
#include "Util/Util.h"

void SkinnedModel::SetFileInfo(FileInfo fileInfo) {
    m_fileInfo = fileInfo;
}

const FileInfo& SkinnedModel::GetFileInfo() {
    return m_fileInfo;
}

LoadingState& SkinnedModel::GetLoadingState() {
    return m_loadingState;
}

void SkinnedModel::SetLoadingState(LoadingState loadingState) {
    m_loadingState = loadingState;
}

const std::string& SkinnedModel::GetName() {
    return m_fileInfo.name;
}

void SkinnedModel::AddMeshIndex(uint32_t index) {
    meshIndices.push_back(index);
}

size_t SkinnedModel::GetMeshCount() {
    return meshIndices.size();
}

std::vector<uint32_t>& SkinnedModel::GetMeshIndices() {
    return meshIndices;
}