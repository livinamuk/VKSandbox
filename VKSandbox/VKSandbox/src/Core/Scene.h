#pragma once
#include "HellTypes.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Debug.h"
#include "Types/AnimatedGameObject.h"
#include "Types/BulletCasing.h"
#include "Types/GameObject.h"
#include "Types/Light.h"
#include "Util/Util.h"
#include "glm/gtx/intersect.hpp"
#include <vector>

namespace Scene {
    void Init();
    void Update(float deltaTime);
    void CreateAnimatedGameObject();
    void CreateGameObject();
    void SetMaterials();
    void CreateGameObjects();
    void SaveMap(const std::string& filepath);

    AnimatedGameObject* GetAnimatedGameObjectByIndex(int32_t index);
    GameObject* GetGameObjectByIndex(int32_t index);
    GameObject* GetGameObjectByName(const std::string& name);
    Light* GetLightByIndex(int32_t index);

    std::vector<AnimatedGameObject>& GetAnimatedGameObjects();
    std::vector<GameObject>& GetGameObjects();

    std::vector<BulletCasing>& GetBulletCasings();
    std::vector<Light>& GetLights();


    std::vector<RenderItem>& GetRenderItems();
    std::vector<RenderItem>& GetRenderItemsBlended();
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded();
    std::vector<RenderItem>& GetRenderItemsHairTopLayer();
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer();
    std::vector<RenderItem>& GetSkinnedRenderItems();

    void AddBulletCasing(BulletCasingCreateInfo createInfo);
    void AddLight(LightCreateInfo createInfo);

}
