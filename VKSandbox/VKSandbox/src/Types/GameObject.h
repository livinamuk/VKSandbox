#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "../Renderer/Types/Model.hpp"

struct GameObject {
    GameObject() = default;
    GameObject(GameObjectCreateInfo createInfo);
    GameObjectCreateInfo GetCreateInfo();

    std::string m_name;
    Model* m_model = nullptr;
    Transform m_transform;
    std::vector<MeshRenderingInfo> m_meshRenderingInfoSet;

    void SetName(const std::string& name);
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetRotationY(float rotation);
    void SetScale(glm::vec3 scale);
    void SetModel(const std::string& name);
    void SetMeshMaterialByMeshName(const std::string& meshName, const std::string& materialName);
    void SetMeshBlendingMode(const std::string& meshName, BlendingMode blendingMode);
    void SetMeshBlendingModes(BlendingMode blendingMode);
    void SetMousePickIndex(int index);
    void PrintMeshNames();
    void UpdateRenderItems();
    
    void BeginFrame();
    void MarkAsSelected();
    bool IsSelected();

    glm::vec3 GetPosition() const;
    glm::vec3 GetEulerRotation() const;
    glm::vec3 GetScale() const;
    const glm::mat4 GetModelMatrix();
    const glm::vec3 GetObjectCenter();
    const glm::vec3 GetObjectCenterOffsetFromOrigin();

    const std::vector<RenderItem>& GetRenderItems();
    const std::vector<RenderItem>& GetRenderItemsBlended();
    const std::vector<RenderItem>& GetRenderItemsAlphaDiscarded();
    const std::vector<RenderItem>& GetRenderItemsHairTopLayer();
    const std::vector<RenderItem>& GetRenderItemsHairBottomLayer();

private:
    int m_mousePickIndex = 0;
    bool m_selected = false;
    std::vector<RenderItem> m_renderItems;
    std::vector<RenderItem> m_renderItemsBlended;
    std::vector<RenderItem> m_renderItemsAlphaDiscarded;
    std::vector<RenderItem> m_renderItemsHairTopLayer;
    std::vector<RenderItem> m_renderItemsHairBottomLayer;
};