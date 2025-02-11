#include "Light.h"
#include "AssetManagement/AssetManager.h"

Light::Light(LightCreateInfo createInfo) {   
    m_position = createInfo.position;
    m_color = createInfo.color;
    m_radius = createInfo.radius;
    m_strength = createInfo.strength;
    m_type = createInfo.type;

    if (m_type == LightType::LAMP_POST) {
        uint32_t modelIndex = AssetManager::GetModelIndexByName("LampPost");
        uint32_t materialIndex = AssetManager::GetMaterialIndexByName("LampPost");
        m_model = AssetManager::GetModelByIndex(modelIndex);
        m_material = AssetManager::GetMaterialByIndex(materialIndex);
    }
}

void Light::UpdateRenderItems() {
    m_renderItems.clear();

    Transform transform;
    transform.position = m_position;

    for (uint32_t meshIndex : m_model->GetMeshIndices()) {
        RenderItem& renderItem = m_renderItems.emplace_back();
        renderItem.mousePickType = (int)EditorObjectType::LIGHT;
        renderItem.mousePickIndex = m_mousePickIndex;
        renderItem.modelMatrix =    transform.to_mat4();
        renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
        renderItem.meshIndex = meshIndex;
        if (m_material) {
            renderItem.baseColorTextureIndex = m_material->m_basecolor;
            renderItem.normalMapTextureIndex = m_material->m_normal;
            renderItem.rmaTextureIndex = m_material->m_rma;
        }
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (mesh->GetName() == "Lamp") {
            renderItem.emissiveR = m_color.r;
            renderItem.emissiveG = m_color.g;
            renderItem.emissiveB = m_color.b;
        }
    }
}

std::vector<RenderItem>& Light::GetRenderItems() {
    return m_renderItems;
}

void Light::SetMousePickIndex(int index) {
    m_mousePickIndex = index;
}

glm::vec3 Light::GetPosition() {
    return m_position;
}

glm::vec3 Light::GetColor() {
    return m_color;
}

float Light::GetStrength() {
    return m_strength;
}

float Light::GetRadius() {
    return m_radius;
}
