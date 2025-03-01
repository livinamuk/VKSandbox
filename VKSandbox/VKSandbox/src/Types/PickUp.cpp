#include "PickUp.h"
#include "AssetManagement/AssetManager.h"
#include "Util.h"

PickUp::PickUp(PickUpCreateInfo createInfo) {
    m_transform.position = createInfo.position;
    m_transform.rotation = createInfo.rotation;
    m_pickUpType = Util::StringToPickUpType(createInfo.pickUpType);

    if (m_pickUpType == PickUpType::SHOTGUN_AMMO_BUCKSHOT) {
        m_model = AssetManager::GetModelByName("Shotgun_AmmoBox");
        m_material = AssetManager::GetMaterialByName("Shotgun_AmmoBox");
    }
    else if (m_pickUpType == PickUpType::SHOTGUN_AMMO_SLUG) {
        m_model = AssetManager::GetModelByName("Shotgun_AmmoBox");
        m_material = AssetManager::GetMaterialByName("Shotgun_AmmoBoxSlug");
    }
}

PickUpCreateInfo PickUp::GetCreateInfo() {
    PickUpCreateInfo createInfo;
    createInfo.position = m_transform.position;
    createInfo.rotation = m_transform.rotation;
    createInfo.pickUpType = Util::PickUpTypeToString(m_pickUpType);
    return createInfo;
}

void PickUp::Update(float deltaTime) {
    m_modelMatrix = m_transform.to_mat4();
    UpdateRenderItems();
}

void PickUp::SetMousePickIndex(int mousePickIndex) {
    m_mousePickIndex = mousePickIndex;
}

void PickUp::SetPosition(glm::vec3 position) {
    m_transform.position = position;
}

void PickUp::UpdateRenderItems() {
    m_renderItems.clear();    
   
    for (int i = 0; i < m_model->GetMeshCount(); i++) {
        Mesh* mesh = AssetManager::GetMeshByIndex(m_model->GetMeshIndices()[i]);
        if (mesh) {
            RenderItem renderItem;
            renderItem.mousePickType = (int)EditorObjectType::PICK_UP;
            renderItem.mousePickIndex = m_mousePickIndex;
            renderItem.modelMatrix = GetModelMatrix();
            renderItem.inverseModelMatrix = glm::inverse(renderItem.modelMatrix);
            renderItem.meshIndex = m_model->GetMeshIndices()[i];       
            renderItem.baseColorTextureIndex = m_material->m_basecolor;
            renderItem.normalMapTextureIndex = m_material->m_normal;
            renderItem.rmaTextureIndex = m_material->m_rma;
            m_renderItems.push_back(renderItem);
        }
    }
}
