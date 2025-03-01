#pragma once
#include "Renderer/Types/SkinnedModel.h"
#include "Renderer/Types/AnimationLayer.h"
#include <unordered_map>

struct MeshRenderingEntry {
    std::string meshName;
    int materialIndex = 0;
    int emissiveColorTexutreIndex = -1;
    bool blendingEnabled = false;
    bool drawingEnabled = true;
    bool renderAsGlass = false;
    int meshIndex = -1;
};

struct JointWorldMatrix {
    const char* name;
    glm::mat4 worldMatrix;
};

struct AnimatedGameObject {

    enum class AnimationMode { BINDPOSE, ANIMATION, RAGDOLL };

private:
    std::vector<RenderItem> m_renderItems;

public:
    std::unordered_map<std::string, unsigned int> m_boneMapping;

    glm::mat4 GetAnimatedTransformByBoneName(const char* name);

    AnimatedTransforms m_animatedTransforms;

    const size_t GetAnimatedTransformCount();
    void UpdateRenderItems();
    std::vector<RenderItem>& GetRenderItems();
    const uint32_t GetVerteXCount();
    std::vector<uint32_t> m_skinnedBufferIndices;
    int GetBoneIndex(const std::string& boneName);
    glm::mat4 GetBoneWorldMatrix(const std::string& boneName);
    glm::vec3 GetBoneWorldPosition(const std::string& boneName);

	void Update(float deltaTime);
	void SetName(std::string name);
	void SetSkinnedModel(std::string skinnedModelName);
	void SetScale(float scale);
	void SetPosition(glm::vec3 position);
	void SetRotationX(float rotation);
	void SetRotationY(float rotation);
	void SetRotationZ(float rotation);
    void PlayAnimation(const std::string& animationName, float speed);
	void PlayAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams = AnimationPlaybackParams());
    void PlayAnimation(const std::vector<std::string>& animationNames, float speed);
    void PlayAnimation(const std::vector<std::string>& animationNames, const AnimationPlaybackParams& playbackParams);
    void PlayAndLoopAnimation(const std::string& animationName, float speed);
	void PlayAndLoopAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams = AnimationPlaybackParams());
    void SetAnimationModeToBindPose();
    void SetMeshMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetMeshMaterialByMeshIndex(int meshIndex, const std::string& materialName);
    void SetMeshToRenderAsGlassByMeshIndex(const std::string& materialName);
    void SetMeshEmissiveColorTextureByMeshName(const std::string& meshName, const std::string& textureName);
	void SetAllMeshMaterials(const std::string& materialName);


	std::string GetName();
	const glm::mat4 GetModelMatrix();
	bool IsAnimationComplete();
    glm::vec3 GetScale();

	SkinnedModel* m_skinnedModel = nullptr;
    int m_skinnedModelIndex = -1;

	Transform _transform;
	float _currentAnimationTime = 0;
	glm::mat4 _cameraMatrix = glm::mat4(1);
	std::vector<MeshRenderingEntry> m_meshRenderingEntries;
    AnimationMode m_animationMode = AnimationMode::BINDPOSE;

    bool useCameraMatrix = false;
    glm::mat4 m_cameraMatrix = glm::mat4(1);
    glm::mat4 m_cameraSpawnMatrix = glm::mat4(1);

    void EnableDrawingForAllMesh();
    void EnableDrawingForMeshByMeshName(const std::string& meshName);
    void DisableDrawingForMeshByMeshName(const std::string& meshName);
    void PrintBoneNames();
    void PrintMeshNames();
    void EnableBlendingByMeshIndex(int index);

    //std::vector<glm::mat4> _debugTransformsA;
    //std::vector<glm::mat4> _debugTransformsB;
    bool _hasRagdoll = false;

    struct BoneDebugInfo {
        const char* name;
        const char* parentName;
        glm::vec3 worldPos;
        glm::vec3 parentWorldPos;
    };

    std::vector<BoneDebugInfo> _debugBoneInfo;
    bool _renderDebugBones = false;


    //glm::vec3 FindClosestParentAnimatedNode(std::vector<JointWorldMatrix>& worldMatrices, int parentIndex);

    void SetBaseTransfromIndex(int index) {
        baseTransformIndex = index;
    }
    int GetBaseTransfromIndex() {
        return baseTransformIndex;
    }

private:

	//void UpdateAnimation(float deltaTime);
	//void CalculateBoneTransforms();

    float GetBlendFactor();

	//Animation* _currentAnimation = nullptr;
	//bool _loopAnimation = false;
	//bool _animationPaused = false;
	//float _animationSpeed = 1.0f;
	std::string m_name;
	//bool _animationIsComplete = true;
    int baseTransformIndex = -1;
    bool m_isGold = true;

public:
    float m_accumulatedBlendingTime = 0.0f;
    float m_totalBlendDuration = 0.0f;
    float m_blendFactor = 0.0f;
    std::vector<glm::mat4> m_LocalBlendedBoneTransforms;
    std::vector<glm::mat4> m_globalBlendedNodeTransforms;
    std::vector<glm::mat4>& GetLocalBlendedBoneTransforms();
    std::vector<glm::mat4>& GetGlobalBlendedNodeTransforms();
    glm::mat4 GetBindPoseByBoneName(const std::string& name);


    uint32_t GetAnimationFrameNumber();                     // the logic in here needs rethinking!!!
    bool AnimationIsPastFrameNumber(int frameNumber);       // the logic in here needs rethinking!!!

    AnimationLayer m_animationLayer;

    void DrawBones(glm::vec3 color);
    void DrawBoneTangentVectors(float size = 0.1f);

    void MakeGold();
    void MakeNotGold();


    void SetExclusiveViewportIndex(int index);
    void SetIgnoredViewportIndex(int index);

    bool AnimationByNameIsComplete(const std::string& name);

    int m_ignoredViewportIndex = -1;
    int m_exclusiveViewportIndex = -1;


    AnimationLayer& GetAnimationLayer() { return m_animationLayer; }
    void SubmitForSkinning();
};