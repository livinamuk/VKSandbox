#include "GameObject.h"
#include "../AssetManagement/AssetManager.h"

GameObject::GameObject(GameObjectCreateInfo createInfo) {
    m_transform.position = createInfo.position;
    m_transform.rotation = createInfo.rotation;
    m_transform.scale = createInfo.scale;
    SetModel(createInfo.modelName);
    SetMeshMaterials(createInfo.baseMaterial);
    for (MeshMaterialInfo& meshMaterialInfo : createInfo.meshMaterialInfo) {
        SetMeshMaterialByMeshName(meshMaterialInfo.meshName, meshMaterialInfo.materialName);
    }
    for (MeshBlendingInfo& meshBlendingInfo : createInfo.meshBlendingInfo) {
        SetMeshBlendingMode(meshBlendingInfo.meshName, meshBlendingInfo.blendingMode);
    }
}

void GameObject::SetPosition(glm::vec3 position) {
    m_transform.position = position;
}

void GameObject::SetRotation(glm::vec3 rotation) {
    m_transform.rotation = rotation;
}

void GameObject::SetRotationY(float rotation) {
    m_transform.rotation.y = rotation;
}

void GameObject::SetScale(glm::vec3 scale) {
    m_transform.scale = scale;
}

void GameObject::SetModel(const std::string& name) {
    m_model = AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName(name.c_str()));
    if (m_model) {
        m_meshMaterialIndices.resize(m_model->GetMeshCount());
        m_meshBlendingModes.resize(m_model->GetMeshCount());
        std::fill(m_meshMaterialIndices.begin(), m_meshMaterialIndices.end(), -1);
    }
    else {
        std::cout << "Failed to set model '" << name << "', it does not exist.\n";
    }
    for (BlendingMode& blendingMode : m_meshBlendingModes) {
        blendingMode = BlendingMode::NONE;
    }
}

void GameObject::SetMeshMaterials(const std::string& materialName) {
    if (m_meshMaterialIndices.empty()) {
        std::cout << "GameObject::SetMeshMaterials() failed because m_meshMaterialIndices was empty, you most likely forget to set the model pointer\n";
        return;
    }

    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (m_model && materialIndex != -1) {
        for (int i = 0; i < m_meshMaterialIndices.size(); i++) {
            m_meshMaterialIndices[i] = materialIndex;
        }
    }
}

void GameObject::SetMeshMaterialByMeshName(const std::string& meshName, const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (m_model && materialIndex != -1) {
        for (int i = 0; i < m_model->GetMeshCount(); i++) {
            if (AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i])->GetName() == meshName) {
                m_meshMaterialIndices[i] = materialIndex;
                return;
            }
        }
    }
    if (!m_model) {
        std::cout << "Tried to call SetMeshMaterialByMeshName() but this GameObject has a nullptr m_model\n";
    }
    else if (materialIndex == -1) {
        //std::cout << "Tried to call SetMeshMaterialByMeshName() but the material index was -1\n";
    }
    else {
        std::cout << "Tried to call SetMeshMaterialByMeshName() but the meshName '" << meshName << "' not found\n";
    }
}

void GameObject::SetMeshBlendingMode(const std::string& meshName, BlendingMode blendingMode) {
    // Range checks
    if (m_meshBlendingModes.empty()) {
        std::cout << "GameObject::SetMeshBlendingMode() failed: m_meshBlendingModes was empty!\n";
        return;
    }
    if (m_model) {
        bool found = false;
        for (int i = 0; i < m_model->GetMeshIndices().size(); i++) {
            Mesh* mesh = AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i]);
            if (mesh && mesh->GetName() == meshName) {
                m_meshBlendingModes[i] = blendingMode;
                found = true;
            }
        }
        if (!found) {
            std::cout << "GameObject::SetMeshBlendingMode() failed: " << meshName << " was not found!\n";
        }
    }
}

void GameObject::SetMeshBlendingModes(BlendingMode blendingMode) {
    if (m_model) {
        for (int i = 0; i < m_model->GetMeshIndices().size(); i++) {
            m_meshBlendingModes[i] = blendingMode;
        }
    }
}

void GameObject::SetMousePickIndex(int index) {
    m_mousePickIndex = index;
}

void GameObject::SetName(const std::string& name) {
    m_name = name;
}

void GameObject::PrintMeshNames() {
    if (m_model) {
        std::cout << m_model->GetName() << "\n";
        for (uint32_t meshIndex : m_model->GetMeshIndices()) {
            Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
            if (mesh) {
                std::cout << "-" << meshIndex << ": " << mesh->GetName() << "\n";
            }
        }
    }
}

void GameObject::UpdateRenderItems() {
    m_renderItems.clear();
    m_renderItemsBlended.clear();
    m_renderItemsAlphaDiscarded.clear();
    m_renderItemsHairTopLayer.clear();
    m_renderItemsHairBottomLayer.clear();
    if (m_model) {
        for (int i = 0; i < m_model->GetMeshCount(); i++) {
            Mesh* mesh = AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i]);
            if (mesh) {
                RenderItem renderItem;
                renderItem.mousePickType = (int)EditorObjectType::GAME_OBJECT;
                renderItem.mousePickIndex = m_mousePickIndex;
                renderItem.modelMatrix = GetModelMatrix();
                renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
                renderItem.meshIndex = m_model->GetMeshIndices()[i];
                Material* material = AssetManager::GetMaterialByIndex(m_meshMaterialIndices[i]);
                if (material) {
                    renderItem.baseColorTextureIndex = material->m_basecolor;
                    renderItem.normalMapTextureIndex = material->m_normal;
                    renderItem.rmaTextureIndex = material->m_rma;
                }
                BlendingMode blendingMode = m_meshBlendingModes[i];
                switch (blendingMode) {
                case BlendingMode::NONE: m_renderItems.push_back(renderItem); break;
                case BlendingMode::BLENDED: m_renderItemsBlended.push_back(renderItem); break;
                case BlendingMode::ALPHA_DISCARDED: m_renderItemsAlphaDiscarded.push_back(renderItem); break;
                case BlendingMode::HAIR_TOP_LAYER: m_renderItemsHairTopLayer.push_back(renderItem); break;
                case BlendingMode::HAIR_UNDER_LAYER: m_renderItemsHairBottomLayer.push_back(renderItem); break;
                default: break;
                }
            }
        }
    }
}

glm::vec3 GameObject::GetPosition() const {
    return m_transform.position;
}

glm::vec3 GameObject::GetEulerRotation() const {
    return m_transform.rotation;
}

glm::vec3 GameObject::GetScale() const {
    return m_transform.scale;
}

const glm::mat4 GameObject::GetModelMatrix() {
    return m_transform.to_mat4();
}

const glm::vec3 GameObject::GetObjectCenter() {
    glm::vec3 aabbCenter = (m_model->m_aabbMin + m_model->m_aabbMax) * 0.5f;
    return GetModelMatrix() * glm::vec4(aabbCenter, 1.0f);
}

const glm::vec3 GameObject::GetObjectCenterOffsetFromOrigin() {
    return GetObjectCenter() - glm::vec3(GetModelMatrix()[3]);
}

std::vector<RenderItem>& GameObject::GetRenderItems() {
    return m_renderItems;
}

std::vector<RenderItem>& GameObject::GetRenderItemsBlended() {
    return m_renderItemsBlended;
}

std::vector<RenderItem>& GameObject::GetRenderItemsAlphaDiscarded() {
    return m_renderItemsAlphaDiscarded;
}

std::vector<RenderItem>& GameObject::GetRenderItemsHairTopLayer() {
    return m_renderItemsHairTopLayer;
}

std::vector<RenderItem>& GameObject::GetRenderItemsHairBottomLayer() {
    return m_renderItemsHairBottomLayer;
}
