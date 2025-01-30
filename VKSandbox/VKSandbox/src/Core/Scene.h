#pragma once

#include "HellTypes.h"
#include <vector>
#include "../AssetManagement/AssetManager.h"
#include "../Player/Player.h"
#include "../Core/Debug.h"
#include "../Input/Input.h"
#include "../Types/GameObject.h"
#include "../Util/Util.h"
#include "glm/gtx/intersect.hpp"

namespace Scene {
    extern std::vector<GameObject> g_gameObjects;
    extern std::vector<RenderItem> g_renderItems;
    extern std::vector<RenderItem> g_renderItemsBlended;
    extern std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    extern std::vector<RenderItem> g_renderItemsHairTopLayer;
    extern std::vector<RenderItem> g_renderItemsHairBottomLayer;

    void Init();
    void Update(float deltaTime);
    void CreateGameObject();
    GameObject* GetGameObjectByName(const std::string& name);
    void SetMaterials();
    void CreateGameObjects();

    std::vector<RenderItem>& GetRenderItems();
    std::vector<RenderItem>& GetRenderItemsBlended();
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded();
    std::vector<RenderItem>& GetRenderItemsHairTopLayer();
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer();
}
