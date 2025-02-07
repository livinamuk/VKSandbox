#pragma once
#include "HellDefines.h"
#include "HellTypes.h"
#include "HellEnums.h"
#include <map>
#include <vector>
#include <assert.h>
#include "Animation.h"
#include "SkinnedMesh.hpp"

struct Joint {
    const char* m_name;
    int m_parentIndex;
    glm::mat4 m_inverseBindTransform;
    glm::mat4 m_currentFinalTransform;
    glm::mat4 m_boneOffset;
};

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

struct BoneInfo {
    glm::mat4 BoneOffset;
    glm::mat4 FinalTransformation;
    glm::mat4 ModelSpace_AnimatedTransform;
    glm::mat4 DebugMatrix_BindPose;
    std::string BoneName;

    BoneInfo() {
        BoneOffset = glm::mat4(0);
        FinalTransformation = glm::mat4(0);
        DebugMatrix_BindPose = glm::mat4(1);
        ModelSpace_AnimatedTransform = glm::mat4(1);
    }
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
        ZERO_MEM(m_Buffers);
        m_NumBones = 0;
    }

    void UpdateBoneTransformsFromAnimation(float animTime, Animation* animation, AnimatedTransforms& animatedTransforms, glm::mat4& outCameraMatrix);
    void UpdateBoneTransformsFromBindPose(AnimatedTransforms& animatedTransforms);
    void CalcInterpolatedScaling(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedScale(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    int FindAnimatedNodeIndex(float AnimationTime, const AnimatedNode* animatedNode);
    const AnimatedNode* FindAnimatedNode(Animation* animation, const char* NodeName);

    std::vector<Joint> m_joints;
    unsigned int m_Buffers[NUM_VBs];
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
