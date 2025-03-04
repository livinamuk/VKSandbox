#pragma once
#include "HellTypes.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Debug.h"
#include "Types/AnimatedGameObject.h"
#include "Types/BulletCasing.h"
#include "Types/GameObject.h"
#include "Types/Light.h"
#include "Types/PickUp.h"
#include "Types/Tree.h"
#include "Util/Util.h"
#include "glm/gtx/intersect.hpp"
#include <vector>

namespace World {
    void NewCampainWorld();

    void Init();
    void LoadMap(const std::string& mapName);
    void LoadSingleSector(const std::string& sectorName);
    void LoadSingleSector(SectorCreateInfo& sectorCreateInfo);

    //void RevertToSavedSector();


    void LoadDeathMatchMap();

    //SectorCreateInfo& GetEditorSectorCreateInfo();

    MapCreateInfo* GetCurrentMapCreateInfo();

    void BeginFrame();
    void Update(float deltaTime);
    void ResetWorld();









    void CreateGameObject();
    void CreateAnimatedGameObject();

    //AnimatedGameObject* GetAnimatedGameObjectByIndex(int32_t index);
    GameObject* GetGameObjectByIndex(int32_t index);
    GameObject* GetGameObjectByName(const std::string& name);
    Light* GetLightByIndex(int32_t index);
    PickUp* GetPickUpByIndex(int32_t index);
    Tree* GetTreeByIndex(int32_t index);

    //std::vector<AnimatedGameObject>& GetAnimatedGameObjects();
    std::vector<GameObject>& GetGameObjects();
    std::vector<BulletCasing>& GetBulletCasings();
    std::vector<Light>& GetLights();
    std::vector<PickUp>& GetPickUps();
    std::vector<Tree>& GetTrees();

    std::vector<RenderItem>& GetRenderItems();
    std::vector<RenderItem>& GetRenderItemsBlended();
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded();
    std::vector<RenderItem>& GetRenderItemsHairTopLayer();
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer();
    std::vector<RenderItem>& GetSkinnedRenderItems();

    void AddBulletCasing(BulletCasingCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddGameObject(GameObjectCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddLight(LightCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddPickUp(PickUpCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddTree(TreeCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
}

/*
    Notes
    
    * Sectors
      - Each sector can or can not have a house
      - There is always a house in the first sector you DON'T spawn in
      - Each sector has a 50% chance of having a house
      - There can never be 2 sectors WITHOUT a house in a row
      - If a sector has no house, then it may be a mermaid shop sector
      - If a sector has no house, then it may be a dense woods sector, aka trees and full of enemies

    * Witches
      - they are a rarer shop than the mermaid shop
      - they sell better items
      - they hover above a fire 

*/