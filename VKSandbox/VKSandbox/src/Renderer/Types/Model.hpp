#pragma once
#include "Common.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "../File/FileFormats.h"
#include <string>
#include <vector>

struct Model {

private:
    std::string m_name = "undefined";
    std::vector<uint32_t> m_meshIndices;
    FileInfo m_fileInfo; 
    LoadingState m_loadingState = LoadingState::AWAITING_LOADING_FROM_DISK;
public:
    glm::vec3 m_aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_aabbMax = glm::vec3(-std::numeric_limits<float>::max());
    bool m_awaitingLoadingFromDisk = true;
    std::string m_fullPath = ""; 
    ModelData m_modelData;

public:

    Model() = default;

    void SetFileInfo(FileInfo fileInfo) {
        m_fileInfo = fileInfo;
    }

    FileInfo GetFileInfo() {
        return m_fileInfo;
    }

    LoadingState& GetLoadingState() {
        return m_loadingState;
    }

    void SetLoadingState(LoadingState loadingState) {
        m_loadingState = loadingState;
    }

    void AddMeshIndex(uint32_t index) {
        m_meshIndices.push_back(index);
    }

    size_t GetMeshCount() {
        return m_meshIndices.size();
    }

    std::vector<uint32_t>& GetMeshIndices() {
        return m_meshIndices;
    }

    void SetName(std::string modelName) {
        m_name = modelName;
    }

    void SetAABB(glm::vec3 aabbMin, glm::vec3 aabbMax) {
        m_aabbMin = aabbMin;
        m_aabbMax = aabbMax;
    }

    const std::string GetName() {
        return m_name;
    }
};