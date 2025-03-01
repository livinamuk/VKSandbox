#include "AnimatedGameObject.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Util.h"

const size_t AnimatedGameObject::GetAnimatedTransformCount() {
    return m_animatedTransforms.local.size();
}

void AnimatedGameObject::UpdateRenderItems() {
    if (!m_skinnedModel) return;

    int meshCount = m_meshRenderingEntries.size();
    m_renderItems.clear();
    for (int i = 0; i < meshCount; i++) {
        if (m_meshRenderingEntries[i].drawingEnabled) {
            RenderItem& renderItem = m_renderItems.emplace_back();
            SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(m_meshRenderingEntries[i].meshIndex);
            Material* material = AssetManager::GetMaterialByIndex(m_meshRenderingEntries[i].materialIndex);
            renderItem.baseColorTextureIndex = material->m_basecolor;
            renderItem.normalMapTextureIndex = material->m_normal;
            renderItem.rmaTextureIndex = material->m_rma;
            renderItem.modelMatrix = GetModelMatrix();
            renderItem.inverseModelMatrix = glm::inverse(GetModelMatrix());
            renderItem.meshIndex = m_skinnedModel->GetMeshIndices()[i];
            renderItem.ignoredViewportIndex = m_ignoredViewportIndex;
            renderItem.exclusiveViewportIndex = m_exclusiveViewportIndex;
            renderItem.baseSkinnedVertex = RenderDataManager::GetBaseSkinnedVertex() + mesh->baseVertexLocal;
        }
    }
    RenderDataManager::IncrementBaseSkinnedVertex(m_skinnedModel->GetVertexCount());
}

std::vector<RenderItem>& AnimatedGameObject::GetRenderItems() {
    return m_renderItems;
}

const uint32_t AnimatedGameObject::GetVerteXCount() {
    if (m_skinnedModel) {
        return m_skinnedModel->GetVertexCount();
    }
    else {
        return 0;
    }
}

float AnimatedGameObject::GetBlendFactor() {
    return m_blendFactor;
}

void AnimatedGameObject::Update(float deltaTime) {
    if (!m_skinnedModel) return;

    if (m_animationMode == AnimationMode::BINDPOSE && m_skinnedModel) {
        m_animationLayer.ClearAllAnimationStates();
    }

    m_animationLayer.Update(deltaTime);

    // Blending
    m_accumulatedBlendingTime += deltaTime;
    float remainingTime = m_totalBlendDuration - m_accumulatedBlendingTime;
    m_blendFactor = remainingTime / m_totalBlendDuration;
    m_blendFactor = glm::clamp(m_blendFactor, 0.0f, 1.0f);
    m_blendFactor = Util::SmoothStep(m_blendFactor);

    m_LocalBlendedBoneTransforms.clear();
    m_globalBlendedNodeTransforms.clear();

    for (int i = 0; i < m_skinnedModel->GetBoneCount(); i++) {
        m_LocalBlendedBoneTransforms.push_back(glm::mat4(1));
    }

    for (int i = 0; i < m_animationLayer.m_globalBlendedNodeTransforms.size(); i++) {
        m_globalBlendedNodeTransforms.push_back(m_animationLayer.m_globalBlendedNodeTransforms[i]);
    }

    // Clean this abomination up!
    for (int i = 0; i < m_skinnedModel->m_nodes.size(); i++) {
        const std::string& nodeName = m_skinnedModel->m_nodes[i].name;
        if (m_skinnedModel->m_boneMapping.find(nodeName) != m_skinnedModel->m_boneMapping.end()) {
            unsigned int boneIndex = m_skinnedModel->m_boneMapping[nodeName];
            if (boneIndex >= 0 && boneIndex < m_globalBlendedNodeTransforms.size()) {
                glm::mat4 boneOffset = m_skinnedModel->m_boneOffsets[boneIndex];
                m_LocalBlendedBoneTransforms[boneIndex] = m_globalBlendedNodeTransforms[i] * boneOffset;
            }
            else {
                std::cout << "your animation shit is broken\n";
            }
        }
    }
}

void AnimatedGameObject::SetMeshMaterialByMeshName(const std::string& meshName, const std::string& materialName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.materialIndex = AssetManager::GetMaterialIndexByName(materialName);
        }
    }
}

void AnimatedGameObject::SetMeshMaterialByMeshIndex(int meshIndex, const std::string& materialName) {
    if (!m_skinnedModel) {
        return;
    }
    if (meshIndex >= 0 && meshIndex < m_meshRenderingEntries.size()) {
        m_meshRenderingEntries[meshIndex].materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    }
}

void AnimatedGameObject::SetMeshToRenderAsGlassByMeshIndex(const std::string& meshName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.renderAsGlass = true;
        }
    }
}

void AnimatedGameObject::SetMeshEmissiveColorTextureByMeshName(const std::string& meshName, const std::string& textureName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.emissiveColorTexutreIndex = AssetManager::GetTextureIndexByName(textureName);
        }
    }
}

void AnimatedGameObject::EnableBlendingByMeshIndex(int meshIndex) {
    if (!m_skinnedModel) {
        return;
    }
    if (meshIndex >= 0 && meshIndex < m_meshRenderingEntries.size()) {
        m_meshRenderingEntries[meshIndex].blendingEnabled = true;
    }
}

void AnimatedGameObject::SetAllMeshMaterials(const std::string& materialName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        meshRenderingEntry.materialIndex =AssetManager::GetMaterialIndexByName(materialName);
    }
}

glm::mat4 AnimatedGameObject::GetBindPoseByBoneName(const std::string& name) {
    for (int i = 0; i < m_skinnedModel->m_nodes.size(); i++) {
        if (m_skinnedModel->m_nodes[i].name == name) {
            return m_skinnedModel->m_nodes[i].inverseBindTransform;
        }
    }
    std::cout << "GetBindPoseByBoneName() failed to find name " << name << "\n";
    return glm::mat4();
}

void AnimatedGameObject::SetAnimationModeToBindPose() {
    m_animationMode = AnimationMode::BINDPOSE;
    m_animationLayer.ClearAllAnimationStates();
}

void AnimatedGameObject::PlayAnimation(const std::string& animationName, float speed) {
    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = speed;
    m_animationMode = AnimationMode::ANIMATION;
    m_animationLayer.PlayAnimation(animationName, params);
}

void AnimatedGameObject::PlayAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams) {
    m_animationMode = AnimationMode::ANIMATION;
    m_animationLayer.PlayAnimation(animationName, playbackParams);
}

void AnimatedGameObject::PlayAnimation(const std::vector<std::string>& animationNames, float speed) {
    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = speed;
    int rand = std::rand() % animationNames.size();
    m_animationLayer.PlayAnimation(animationNames[rand], params);
}

void AnimatedGameObject::PlayAnimation(const std::vector<std::string>& animationNames, const AnimationPlaybackParams& playbackParams) {
    int rand = std::rand() % animationNames.size();
    m_animationLayer.PlayAnimation(animationNames[rand], playbackParams);
}

void AnimatedGameObject::PlayAndLoopAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams) {
    m_animationMode = AnimationMode::ANIMATION;
    m_animationLayer.PlayAndLoopAnimation(animationName, playbackParams);
}

void AnimatedGameObject::PlayAndLoopAnimation(const std::string& animationName, float speed) {
    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = speed;
    m_animationMode = AnimationMode::ANIMATION;
    m_animationLayer.PlayAndLoopAnimation(animationName, params);
}


std::vector<glm::mat4>& AnimatedGameObject::GetLocalBlendedBoneTransforms() {
    return m_LocalBlendedBoneTransforms;
}

std::vector<glm::mat4>& AnimatedGameObject::GetGlobalBlendedNodeTransforms() {
    return m_globalBlendedNodeTransforms;
}

const glm::mat4 AnimatedGameObject::GetModelMatrix() {

    if (useCameraMatrix) {
        return m_cameraMatrix;
    }

    if (m_animationMode == AnimationMode::RAGDOLL) {
        return glm::mat4(1);
    }
    else {
        return _transform.to_mat4();
    }
}

bool AnimatedGameObject::IsAnimationComplete() {
    std::cout << "WARNING!!! AnimatedGameObject::IsAnimationComplete() is not implemeneted!\n";
    return true;
    //return m_animationStateA.IsComplete();// _animationIsComplete;
}

std::string AnimatedGameObject::GetName() {
    return m_name;
}

void AnimatedGameObject::SetName(std::string name) {
    m_name = name;
}

void AnimatedGameObject::SetSkinnedModel(std::string name) {
    SkinnedModel* ptr = AssetManager::GetSkinnedModelByName(name);
    if (ptr) {
        m_skinnedModelIndex = AssetManager::GetSkinnedModelIndexByName(name);
        m_animationLayer.SetSkinnedModel(name);
        m_skinnedModel = ptr;
        m_meshRenderingEntries.clear();

        int meshCount = m_skinnedModel->GetMeshCount();

        for (int i = 0; i < meshCount; i++) {
            SkinnedMesh* skinnedMesh = AssetManager::GetSkinnedMeshByIndex(m_skinnedModel->GetMeshIndices()[i]);
            MeshRenderingEntry& meshRenderingEntry = m_meshRenderingEntries.emplace_back();
            meshRenderingEntry.meshName = skinnedMesh->name;
            meshRenderingEntry.meshIndex = m_skinnedModel->GetMeshIndices()[i];
        }

        // Store bone indices
        m_boneMapping.clear();
        for (int i = 0; i < m_skinnedModel->m_nodes.size(); i++) {
            m_boneMapping[m_skinnedModel->m_nodes[i].name] = i;
        }
    }
    else {
        std::cout << "Could not SetSkinnedModel(name) with name: \"" << name << "\", it does not exist\n";
    }
}

glm::mat4 AnimatedGameObject::GetAnimatedTransformByBoneName(const char* name) {
    int index = m_boneMapping[name];
    if (index >= 0 && index < m_animationLayer.m_globalBlendedNodeTransforms.size()) {
        return m_animationLayer.m_globalBlendedNodeTransforms[index];
    }
    //std::cout << "AnimatedGameObject::GetAnimatedTransformByBoneName() FAILED, no matching bone name: " << name << "\n";
    return glm::mat4(1);
}


glm::vec3 AnimatedGameObject::GetScale() {
    return  _transform.scale;
}

void AnimatedGameObject::SetScale(float scale) {
    _transform.scale = glm::vec3(scale);
}
void AnimatedGameObject::SetPosition(glm::vec3 position) {
    _transform.position = position;
}

void AnimatedGameObject::SetRotationX(float rotation) {
    _transform.rotation.x = rotation;
}

void AnimatedGameObject::SetRotationY(float rotation) {
    _transform.rotation.y = rotation;
}

void AnimatedGameObject::SetRotationZ(float rotation) {
    _transform.rotation.z = rotation;
}

void AnimatedGameObject::EnableDrawingForAllMesh() {
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        meshRenderingEntry.drawingEnabled = true;
    }
}

void AnimatedGameObject::EnableDrawingForMeshByMeshName(const std::string& meshName) {
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.drawingEnabled = true;
            return;
        }
    }
    //std::cout << "DisableDrawingForMeshByMeshName() called but name " << meshName << " was not found!\n";
}

void AnimatedGameObject::DisableDrawingForMeshByMeshName(const std::string& meshName) {
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.drawingEnabled = false;
            return;
        }
    }
    //std::cout << "DisableDrawingForMeshByMeshName() called but name " << meshName << " was not found!\n";
}

void AnimatedGameObject::PrintBoneNames() {
    std::cout << m_skinnedModel->GetName() << "\n";
    for (int i = 0; i < m_skinnedModel->m_nodes.size(); i++) {
        std::cout << "-" << i << " " << m_skinnedModel->m_nodes[i].name << "\n";
    }
}

void AnimatedGameObject::PrintMeshNames() {
    std::cout << m_skinnedModel->GetName() << "\n";
    for (int i = 0; i < m_meshRenderingEntries.size(); i++) {
        std::cout << "-" << i << " " << m_meshRenderingEntries[i].meshName << "\n";
    }
}

uint32_t AnimatedGameObject::GetAnimationFrameNumber() {
    if (m_animationLayer.m_animationStates.size()) {
        return m_animationLayer.m_animationStates[0].GetAnimationFrameNumber();
    }
    return 0;
}

bool AnimatedGameObject::AnimationIsPastFrameNumber(int frameNumber) {
    return frameNumber < GetAnimationFrameNumber();
}

void AnimatedGameObject::DrawBones(glm::vec3 color) {
    for (const glm::mat4& boneWorldMatrix : m_animationLayer.m_globalBlendedNodeTransforms) {
        glm::vec3 position = GetModelMatrix() * boneWorldMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        Renderer::DrawPoint(position, color);
    }
}

void AnimatedGameObject::DrawBoneTangentVectors(float size) {    
    // Hack to not render them for view weapon
    for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
        Player* player = Game::GetLocalPlayerByIndex(i);
        if (player->GetViewWeaponAnimatedGameObject() == this) {
            return;
        }
    }
    // Otherwise sent the bone tangent vectors to the render as lines
    for (const glm::mat4& boneWorldMatrix : m_animationLayer.m_globalBlendedNodeTransforms) {
        glm::vec3 origin = GetModelMatrix() * boneWorldMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 right = glm::normalize(glm::vec3(boneWorldMatrix[0]));
        glm::vec3 up = glm::normalize(glm::vec3(boneWorldMatrix[1]));
        glm::vec3 forward = glm::normalize(glm::vec3(boneWorldMatrix[2]));
        Renderer::DrawLine(origin, origin + (forward * size), BLUE);
        Renderer::DrawLine(origin, origin + (up * size), GREEN);
        Renderer::DrawLine(origin, origin + (right * size), RED);
    }
}

void AnimatedGameObject::MakeGold() {
    m_isGold = true;
}
void AnimatedGameObject::MakeNotGold() {
    m_isGold = false;
}

void AnimatedGameObject::SetExclusiveViewportIndex(int index) {
    m_exclusiveViewportIndex = index;
}

void AnimatedGameObject::SetIgnoredViewportIndex(int index) {
    m_ignoredViewportIndex = index;
}

bool AnimatedGameObject::AnimationByNameIsComplete(const std::string& name) {
    for (AnimationState& AnimationState : m_animationLayer.m_animationStates) {
        int animationIndex = AssetManager::GetAnimationIndexByName(name);
        if (AnimationState.m_index == animationIndex) {
            return AnimationState.IsComplete();
        }
    }
    return true;
}

int AnimatedGameObject::GetBoneIndex(const std::string& boneName) {
    auto it = m_boneMapping.find(boneName);
    return (it != m_boneMapping.end()) ? it->second : -1;
}

glm::mat4 AnimatedGameObject::GetBoneWorldMatrix(const std::string& boneName) {
    int boneIndex = GetBoneIndex(boneName);
    if (boneIndex == -1 || m_animationLayer.m_globalBlendedNodeTransforms.empty()) {
        return glm::mat4(1.0f);
    }
    else {
        return GetModelMatrix() * m_animationLayer.m_globalBlendedNodeTransforms[boneIndex];
    }
}

glm::vec3 AnimatedGameObject::GetBoneWorldPosition(const std::string& boneName) {
    return GetBoneWorldMatrix(boneName)[3];
}


void AnimatedGameObject::SubmitForSkinning() {
    RenderDataManager::SubmitAnimatedGameObjectForSkinning(this);
}