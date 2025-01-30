#pragma once
#include "HellTypes.h"
#include "../Renderer/Types/Model.hpp"

struct GameObject {
    std::string m_name;
    Model* m_model = nullptr;
    Transform m_transform;
    std::vector<BlendingMode> m_meshBlendingModes;
    std::vector<int> m_meshMaterialIndices;

    void SetName(const std::string& name);
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetRotationY(float rotation);
    void SetModel(const std::string& name);
    void SetMeshMaterials(const char* materialName);
    void SetMeshMaterialByMeshName(std::string meshName, const char* materialName);
    void SetMeshBlendingMode(const char* meshName, BlendingMode blendingMode);
    void SetMeshBlendingModes(BlendingMode blendingMode);
    void SetMousePickIndex(int index);
    void PrintMeshNames();
    void UpdateRenderItems();

    std::vector<RenderItem>& GetRenderItems();
    std::vector<RenderItem>& GetRenderItemsBlended();
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded();
    std::vector<RenderItem>& GetRenderItemsHairTopLayer();
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer();

private:
    int m_mousePickIndex = 0;
    std::vector<RenderItem> m_renderItems;
    std::vector<RenderItem> m_renderItemsBlended;
    std::vector<RenderItem> m_renderItemsAlphaDiscarded;
    std::vector<RenderItem> m_renderItemsHairTopLayer;
    std::vector<RenderItem> m_renderItemsHairBottomLayer;
};