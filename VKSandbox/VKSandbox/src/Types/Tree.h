#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Renderer/Types/Model.hpp"

struct Tree {
    Tree() = default;
    Tree(TreeCreateInfo createInfo);
    TreeCreateInfo GetCreateInfo();
    void SetPosition(glm::vec3 position);
    void BeginFrame();
    void MarkAsSelected();
    void Update(float deltaTime);
    void UpdateRenderItems();
    void SetMousePickIndex(int mousePickIndex);

    bool IsSelected();

    const std::vector<RenderItem>& GetRenderItems() { return m_renderItems; }
    const glm::vec3& GetPosition() { return m_transform.position; }
    const glm::vec3& GetRotation() { return m_transform.rotation; }
    const glm::vec3& GetScale() { return m_transform.scale; }
    const glm::mat4& GetModelMatrix() { return m_modelMatrix; }

private:
    Model* m_model = nullptr;
    Material* m_material = nullptr;
    TreeType m_treeType = TreeType::UNDEFINED;
    Transform m_transform;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    std::vector<RenderItem> m_renderItems;
    int m_mousePickIndex = 0;
    bool m_isSelected = false;
};