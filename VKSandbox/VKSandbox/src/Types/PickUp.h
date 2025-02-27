#pragma once
#include "HellTypes.h"
#include "HellEnums.h"
#include "CreateInfo.h"
#include "Renderer/Types/Model.hpp"

struct PickUp {
    PickUp() = default;
    PickUp(PickUpCreateInfo createInfo); 
    void SetPosition(glm::vec3 position);
    void Update(float deltaTime);
    void UpdateRenderItems();
    void SetMousePickIndex(int mousePickIndex);

    const std::vector<RenderItem>& GetRenderItems() { return m_renderItems; }
    const glm::vec3& GetPosition() { return m_transform.position; }
    const glm::vec3& GetRotation() { return m_transform.rotation; }
    const glm::mat4& GetModelMatrix() { return m_modelMatrix; }

private:
    Model* m_model = nullptr;
    Material* m_material = nullptr;
    PickUpType m_pickUpType = PickUpType::UNDEFINED;
    Transform m_transform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    std::vector<RenderItem> m_renderItems;
    int m_mousePickIndex = 0;
};