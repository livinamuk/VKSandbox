#pragma once
#include "HellDefines.h"
#include "HellTypes.h"
#include "HellEnums.h"
#include <map>
#include <vector>
#include <assert.h>
#include "Animation.h"
#include "SkinnedMesh.hpp"

struct AnimatedTransforms {
    std::vector<glm::mat4> local;
    std::vector<glm::mat4> worldspace;
    std::vector<std::string> names; // temp for debugging

    void Resize(int size) {
        local.resize(size);
        worldspace.resize(size);
        names.resize(size);
    }

    const size_t GetSize() {
        return local.size();
    }
};


struct Node {
    const char* m_name;
    int m_parentIndex;
    glm::mat4 m_inverseBindTransform;
    glm::mat4 m_currentFinalTransform;
    glm::mat4 m_boneOffset;
};

struct Bone {
    std::string name = "";
    glm::mat4 inverseBindTransform = glm::mat4(1);
    int parentIndex = -1;
};

struct BoneInfo {
    glm::mat4 BoneInfoOffset = glm::mat4(0);
    std::string BoneName = "";
};

struct VertexBoneData {
    unsigned int IDs[4];
    float Weights[4];
    VertexBoneData() {
        Reset();
    };
    void Reset() {
        ZERO_MEM(IDs);
        ZERO_MEM(Weights);
    }
    void AddBoneData(unsigned int BoneID, float Weight) {
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        return;
    }
};

struct SkinnedModel {

private:
    std::vector<uint32_t> meshIndices;
public:

    SkinnedModel() {
        //ZERO_MEM(m_Buffers);
        m_NumBones = 0;
    }

    std::vector<Node> m_nodes;
    std::vector<Bone> m_bones; 
    
    std::map<std::string, unsigned int> m_BoneMapping;
    unsigned int m_NumBones;
    std::vector<BoneInfo> m_BoneInfo;
    glm::mat4 m_GlobalInverseTransform = glm::mat4(1);
    uint32_t m_vertexCount = 0;

    void AddMeshIndex(uint32_t index);
    size_t GetMeshCount();
    std::vector<uint32_t>& GetMeshIndices();
    void SetFileInfo(FileInfo fileInfo);
    void SetLoadingState(LoadingState loadingState);
    const FileInfo& GetFileInfo();
    const std::string& GetName();
    LoadingState& GetLoadingState();

private:
    LoadingState m_loadingState = LoadingState::AWAITING_LOADING_FROM_DISK;
    FileInfo m_fileInfo;
};
