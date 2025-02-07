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
    m_skinnedMeshRenderItems.clear();
    for (int i = 0; i < meshCount; i++) {
        if (m_meshRenderingEntries[i].drawingEnabled) {
            SkinnedRenderItem& renderItem = m_skinnedMeshRenderItems.emplace_back();
            SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(m_meshRenderingEntries[i].meshIndex);
            Material* material = AssetManager::GetMaterialByIndex(m_meshRenderingEntries[i].materialIndex);
            renderItem.baseColorTextureIndex = material->m_basecolor;
            renderItem.normalTextureIndex = material->m_normal;
            renderItem.rmaTextureIndex = material->m_rma;
            renderItem.modelMatrix = GetModelMatrix();
            renderItem.inverseModelMatrix = glm::inverse(GetModelMatrix());
            renderItem.srcMeshIndex = m_skinnedModel->GetMeshIndices()[i];
            renderItem.baseVertex = RenderDataManager::GetBaseSkinnedVertex() + mesh->baseVertexLocal;
        }
    }
    RenderDataManager::IncrementBaseSkinnedVertex(m_skinnedModel->m_vertexCount);
}

std::vector<SkinnedRenderItem>& AnimatedGameObject::GetRenderItems() {
    return m_skinnedMeshRenderItems;
}

void AnimatedGameObject::SetFlag(Flag flag) {
    m_flag = flag;
}

void AnimatedGameObject::SetPlayerIndex(int32_t index) {
    m_playerIndex = index;
}

const AnimatedGameObject::Flag AnimatedGameObject::GetFlag() {
    return m_flag;
}

const int32_t AnimatedGameObject::GetPlayerIndex() {
    return m_playerIndex;
}

const uint32_t AnimatedGameObject::GetVerteXCount() {
    if (m_skinnedModel) {
        return m_skinnedModel->m_vertexCount;
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
        m_skinnedModel->UpdateBoneTransformsFromBindPose(m_animatedTransforms);
        m_animationLayer.ClearAllAnimationStates();
    }

    m_animationLayer.Update(deltaTime);

    // Blending
    m_accumulatedBlendingTime += deltaTime;
    float remainingTime = m_totalBlendDuration - m_accumulatedBlendingTime;
    m_blendFactor = remainingTime / m_totalBlendDuration;
    m_blendFactor = glm::clamp(m_blendFactor, 0.0f, 1.0f);
    m_blendFactor = Util::SmoothStep(m_blendFactor);

    //m_animationStateA.Update(m_skinnedModelIndex, deltaTime);
    //m_animationStateB.Update(m_skinnedModelIndex, deltaTime);

    m_LocalBlendedBoneTransforms.clear();
    m_globalBlendedNodeTransforms.clear();

    for (int i = 0; i < m_skinnedModel->m_NumBones; i++) {
        m_LocalBlendedBoneTransforms.push_back(glm::mat4(1));
    }
    // for (int i = 0; i < m_animationStateA.m_globalNodeTransforms.size(); i++) {
   //     m_globalBlendedNodeTransforms.push_back(m_animationStateA.m_globalNodeTransforms[i].to_mat4());
   // }

    for (int i = 0; i < m_animationLayer.m_globalBlendedNodeTransforms.size(); i++) {
        m_globalBlendedNodeTransforms.push_back(m_animationLayer.m_globalBlendedNodeTransforms[i]);
    }

    /*
    if (m_animationStateB.GetAnimationIndex() != -1) {
        m_globalBlendedNodeTransforms.clear();
        for (int i = 0; i < m_animationStateB.m_globalNodeTransforms.size(); i++) {
            AnimatedTransform transform = Util::BlendTransforms(m_animationStateA.m_globalNodeTransforms[i], m_animationStateB.m_globalNodeTransforms[i], m_blendFactor);
            m_globalBlendedNodeTransforms.push_back(transform.to_mat4());
        }
    }*/

    for (int i = 0; i < m_skinnedModel->m_joints.size(); i++) {
        const char* NodeName = m_skinnedModel->m_joints[i].m_name;
        if (m_skinnedModel->m_BoneMapping.find(NodeName) != m_skinnedModel->m_BoneMapping.end()) {
            unsigned int boneIndex = m_skinnedModel->m_BoneMapping[NodeName];
            if (boneIndex >= 0 && boneIndex < m_globalBlendedNodeTransforms.size()) {
                m_LocalBlendedBoneTransforms[boneIndex] = m_globalBlendedNodeTransforms[i] * m_skinnedModel->m_joints[i].m_boneOffset;
            }
            else {
                std::cout << "your animation shit is broken\n";
            }
        }
    }
}

void AnimatedGameObject::SetMeshMaterialByMeshName(std::string meshName, std::string materialName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.materialIndex = AssetManager::GetMaterialIndex(materialName);
        }
    }
}

void AnimatedGameObject::SetMeshMaterialByMeshIndex(int meshIndex, std::string materialName) {
    if (!m_skinnedModel) {
        return;
    }
    if (meshIndex >= 0 && meshIndex < m_meshRenderingEntries.size()) {
        m_meshRenderingEntries[meshIndex].materialIndex = AssetManager::GetMaterialIndex(materialName);
    }
}

void AnimatedGameObject::SetMeshToRenderAsGlassByMeshIndex(std::string meshName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.renderAsGlass = true;
        }
    }
}

void AnimatedGameObject::SetMeshEmissiveColorTextureByMeshName(std::string meshName, std::string textureName) {
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

glm::mat4 AnimatedGameObject::GetJointWorldTransformByName(const char* jointName) {
    for (int i = 0; i < m_jointWorldMatrices.size(); i++) {
        if (jointName != "undefined" && Util::StrCmp(m_jointWorldMatrices[i].name, jointName)) {
            return GetModelMatrix() * m_jointWorldMatrices[i].worldMatrix;
        }
    }
    std::cout << "AnimatedGameObject::GetJointWorldTransformByName() failed, could not find bone name " << jointName << "\n";
    return glm::mat4();
}



void AnimatedGameObject::SetAllMeshMaterials(std::string materialName) {
    if (!m_skinnedModel) {
        return;
    }
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        meshRenderingEntry.materialIndex =AssetManager::GetMaterialIndex(materialName);
    }
}

glm::mat4 AnimatedGameObject::GetBindPoseByBoneName(const std::string& name) {
    for (int i = 0; i < m_skinnedModel->m_joints.size(); i++) {
        if (m_skinnedModel->m_joints[i].m_name == name) {
            return m_skinnedModel->m_joints[i].m_inverseBindTransform;
        }
    }
    std::cout << "GetBindPoseByBoneName() failed to find name " << name << "\n";
    return glm::mat4();
}

void AnimatedGameObject::SetAnimationModeToBindPose() {
    m_animationMode = AnimationMode::BINDPOSE;
    m_animationLayer.ClearAllAnimationStates();
}

void AnimatedGameObject::PlayAnimation(std::string animationName, const AnimationPlaybackParams& playbackParams) {
    m_animationMode = AnimationMode::ANIMATION;
    m_animationLayer.PlayAnimation(animationName, playbackParams);
}

void AnimatedGameObject::PlayAndLoopAnimation(std::string animationName, const AnimationPlaybackParams& playbackParams) {
    m_animationMode = AnimationMode::ANIMATION;
    m_animationLayer.PlayAndLoopAnimation(animationName, playbackParams);
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
        for (int i = 0; i < m_skinnedModel->m_joints.size(); i++) {
            m_boneMapping[m_skinnedModel->m_joints[i].m_name] = i;
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


bool AnimatedGameObject::AnimationIsPastPercentage(float percent) {
    /*if (!_currentAnimation) {
        return false; // REMOVE ONCE YOU HAVE VULKAN LOADING SHIT CORRECTLY!
    }
    if (_currentAnimationTime * _currentAnimation->GetTicksPerSecond() > _currentAnimation->m_duration * (percent / 100.0))
        return true;
    else*/
        return false;
}

glm::vec3 AnimatedGameObject::GetAK74USCasingSpawnPostion() {
    if (m_name == "AKS74U") {
        int boneIndex = m_skinnedModel->m_BoneMapping["Weapon"];
        if (m_animatedTransforms.worldspace.size()) {
            glm::mat4 boneMatrix = m_animatedTransforms.worldspace[boneIndex];
            Transform offset;
            offset.position = glm::vec3(-2.0f, 2.0f, -2.0f); // real

            static float x2 = -44.0f;
            static float y2 = -28.0f;
            static float z2 = 175.0f;
            float amount = 0.5f;
            if (Input::KeyDown(HELL_KEY_LEFT)) {
                x2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT)) {
                x2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_LEFT_BRACKET)) {
                y2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT_BRACKET)) {
                y2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_UP)) {
                z2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_DOWN)) {
                z2 += amount;
            }
             //std::cout << x2 << ", " << y2 << ", " << z2 << "\n";

             //offset.position = glm::vec3(x2, y2, z2); // hack to look good
            offset.position = glm::vec3(-95, -54, 236.5); // hack to look good


            glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
            float x = m[3][0];
            float y = m[3][1];
            float z = m[3][2];
            return glm::vec3(x, y, z);
        }
    }
    return glm::vec3(0);
}

glm::vec3 AnimatedGameObject::GetGlockCasingSpawnPostion() {
    if (!m_skinnedModel) {
        return glm::vec3(0); // REMOVE ONCE YOU HAVE VULKAN LOADING SHIT CORRECTLY!
    }
    if (m_name == "Glock") {
        int boneIndex = m_skinnedModel->m_BoneMapping["Barrel"];
        if (m_animatedTransforms.worldspace.size()) {
            glm::mat4 boneMatrix = m_animatedTransforms.worldspace[boneIndex];
            Transform offset;
            offset.position = glm::vec3(-2.0f, 2.0f, -2.0f); // real

            static float x2 = -44.0f;
            static float y2 = -28.0f;
            static float z2 = 175.0f;
            float amount = 0.5f;
            if (Input::KeyDown(HELL_KEY_LEFT)) {
                x2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT)) {
                x2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_LEFT_BRACKET)) {
                y2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_RIGHT_BRACKET)) {
                y2 += amount;
            }
            if (Input::KeyDown(HELL_KEY_UP)) {
                z2 -= amount;
            }
            if (Input::KeyDown(HELL_KEY_DOWN)) {
                z2 += amount;
            }
          // std::cout << x2 << ", " << y2 << ", " << z2 << "\n";

           // offset.position = glm::vec3(x2, y2, z2); // hack to look good
            offset.position = glm::vec3(-90.5, -29.5, 267.5); // hack to look good

            glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
            float x = m[3][0];
            float y = m[3][1];
            float z = m[3][2];
            return glm::vec3(x, y, z);
        }
    }
    return glm::vec3(0);
}



glm::vec3 AnimatedGameObject::GetAKS74UBarrelPostion() {
    if (m_name == "AKS74U") {
        int boneIndex = m_skinnedModel->m_BoneMapping["Weapon"];
        glm::mat4 boneMatrix = m_animatedTransforms.worldspace[boneIndex];
        Transform offset;
        offset.position = glm::vec3(0, 0 + 1, 36);
        glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
        float x = m[3][0];
        float y = m[3][1];
        float z = m[3][2];
        return glm::vec3(x, y, z);
    }
    else {
        return glm::vec3(0);
    }
}

glm::vec3 AnimatedGameObject::GetShotgunBarrelPosition() {
    int boneIndex = m_skinnedModel->m_BoneMapping["Weapon"];
    glm::mat4 boneMatrix = m_animatedTransforms.worldspace[boneIndex];
    Transform offset;
    offset.position = glm::vec3(82, 2, -10);
    glm::mat4 m = GetModelMatrix() * boneMatrix * offset.to_mat4();
    float x = m[3][0];
    float y = m[3][1];
    float z = m[3][2];
    return glm::vec3(x, y, z);
}

void AnimatedGameObject::EnableDrawingForAllMesh() {
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        meshRenderingEntry.drawingEnabled = true;
    }
}

void AnimatedGameObject::EnableDrawingForMeshByMeshName(std::string meshName) {
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.drawingEnabled = true;
            return;
        }
    }
    //std::cout << "DisableDrawingForMeshByMeshName() called but name " << meshName << " was not found!\n";
}

void AnimatedGameObject::DisableDrawingForMeshByMeshName(std::string meshName) {
    for (MeshRenderingEntry& meshRenderingEntry : m_meshRenderingEntries) {
        if (meshRenderingEntry.meshName == meshName) {
            meshRenderingEntry.drawingEnabled = false;
            return;
        }
    }
    //std::cout << "DisableDrawingForMeshByMeshName() called but name " << meshName << " was not found!\n";
}

void AnimatedGameObject::PrintMeshNames() {
    std::cout << m_skinnedModel->GetName() << "\n";
    for (int i = 0; i < m_meshRenderingEntries.size(); i++) {
        std::cout << "-" << i << " " << m_meshRenderingEntries[i].meshName << "\n";
    }
}

uint32_t AnimatedGameObject::GetAnimationFrameNumber() {
    /*if (_currentAnimation) {
        return  _currentAnimationTime * _currentAnimation->m_ticksPerSecond;
    }
    else {*/
        return 0;
   // }
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

//void AnimatedGameObject::DrawMeshAABBs(glm::vec3 color) {
//    for (uint32_t& meshIndex : m_skinnedModel->GetMeshIndices()) {
//        SkinnedMesh* skinnedMesh = AssetManager::GetSkinnedMeshByIndex(meshIndex);
//        AABB aabb = Util::ComputeWorldAABB(skinnedMesh->aabbMin, skinnedMesh->aabbMax, modelMatrix);
//        // hmm damn, these mesh are skinned...  
//   }
//}