#include "World.h"
#include "CreateInfo.h"
#include "HellDefines.h"
#include "HellTypes.h"
#include "Util.h"

#include "Core/Audio.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "World/MapManager.h"
#include "World/SectorManager.h"

namespace World {
    std::vector<Light> g_lights;
    std::vector<AnimatedGameObject> g_animatedGameObjects;
    std::vector<BulletCasing> g_bulletCasings;
    std::vector<GameObject> g_gameObjects;
    std::vector<PickUp> g_pickUps;
    std::vector<Tree> g_trees;

    std::vector<RenderItem> g_renderItems;
    std::vector<RenderItem> g_renderItemsBlended;
    std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    std::vector<RenderItem> g_renderItemsHairTopLayer;
    std::vector<RenderItem> g_renderItemsHairBottomLayer;
    std::vector<RenderItem> g_skinnedRenderItems;

    std::string g_currentMapName = "";

    void AddSectorAtLocation(SectorCreateInfo& sectorCreateInfo, SpawnOffset spawnOffset);

    void Init() {
        LoadMap("TestMap");
    }

    void BeginFrame() {
        for (GameObject& gameObject : g_gameObjects) {
            gameObject.BeginFrame();
        }
        for (Tree& tree : g_trees) {
            tree.BeginFrame();
        }
    }

    void Update(float deltaTime) {

       //if (Input::KeyPressed(HELL_KEY_SPACE)) {
       //    if (OpenGLRenderer::IsMouseRayWorldPositionReadBackReady()) {
       //        TreeCreateInfo treeCreateInfo;
       //        treeCreateInfo.position = OpenGLRenderer::GetMouseRayWorldPostion();
       //        treeCreateInfo.type = (int)TreeType::TREE_LARGE_0;
       //        g_editorSector.trees.push_back(treeCreateInfo);
       //        AddTree(treeCreateInfo);
       //        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
       //    }
       //}

        for (GameObject& gameObject : g_gameObjects) {
            gameObject.UpdateRenderItems();

            // Selected outline?
            if (gameObject.IsSelected()) {
                RenderDataManager::SubmitForOutlineRendering(gameObject.GetRenderItems());
                RenderDataManager::SubmitForOutlineRendering(gameObject.GetRenderItemsHairTopLayer());
                RenderDataManager::SubmitForOutlineRendering(gameObject.GetRenderItemsHairBottomLayer());
                break;
            }
        }
        for (AnimatedGameObject& animatedGameObject : g_animatedGameObjects) {
            animatedGameObject.Update(deltaTime);
        }

        // Clear global render item vectors
        g_renderItems.clear();
        g_skinnedRenderItems.clear();
        g_renderItemsBlended.clear();
        g_renderItemsAlphaDiscarded.clear();
        g_renderItemsHairTopLayer.clear();
        g_renderItemsHairBottomLayer.clear();

        int mousePickIndex = 0;
        for (PickUp& pickUp : g_pickUps) {
            pickUp.SetMousePickIndex(mousePickIndex++);
            pickUp.Update(deltaTime);
            g_renderItems.insert(g_renderItems.end(), pickUp.GetRenderItems().begin(), pickUp.GetRenderItems().end());
        }

        // Trees
        mousePickIndex = 0;
        for (Tree& tree : g_trees) {
            tree.SetMousePickIndex(mousePickIndex++);
            tree.Update(deltaTime);
            g_renderItems.insert(g_renderItems.end(), tree.GetRenderItems().begin(), tree.GetRenderItems().end());
        
            // Selected outline?
            if (tree.IsSelected()) {
                RenderDataManager::SubmitForOutlineRendering(tree.GetRenderItems());
            }
        
        }

        // Update each GameObject and collect render items
        mousePickIndex = 0;
        for (GameObject& gameObject : g_gameObjects) {
            gameObject.SetMousePickIndex(mousePickIndex++);
            gameObject.UpdateRenderItems();
            // Merge render items into global vectors
            g_renderItems.insert(g_renderItems.end(), gameObject.GetRenderItems().begin(), gameObject.GetRenderItems().end());
            g_renderItemsBlended.insert(g_renderItemsBlended.end(), gameObject.GetRenderItemsBlended().begin(), gameObject.GetRenderItemsBlended().end());
            g_renderItemsAlphaDiscarded.insert(g_renderItemsAlphaDiscarded.end(), gameObject.GetRenderItemsAlphaDiscarded().begin(), gameObject.GetRenderItemsAlphaDiscarded().end());
            g_renderItemsHairTopLayer.insert(g_renderItemsHairTopLayer.end(), gameObject.GetRenderItemsHairTopLayer().begin(), gameObject.GetRenderItemsHairTopLayer().end());
            g_renderItemsHairBottomLayer.insert(g_renderItemsHairBottomLayer.end(), gameObject.GetRenderItemsHairBottomLayer().begin(), gameObject.GetRenderItemsHairBottomLayer().end());
        }

        // Lights
        mousePickIndex = 0;
        for (Light& light : g_lights) {
            light.UpdateRenderItems();
            light.SetMousePickIndex(mousePickIndex++);
            g_renderItems.insert(g_renderItems.end(), light.GetRenderItems().begin(), light.GetRenderItems().end());
        }

        for (BulletCasing& casing : g_bulletCasings) {
            RenderItem& renderItem = g_renderItems.emplace_back();
            renderItem.modelMatrix = casing.GetModelMatrix();
            renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
            Material* material = AssetManager::GetMaterialByIndex(casing.GetMaterialIndex());
            renderItem.baseColorTextureIndex = material->m_basecolor;
            renderItem.rmaTextureIndex = material->m_rma;
            renderItem.normalMapTextureIndex = material->m_normal;
            renderItem.meshIndex = casing.GetMeshIndex();
        }


        RenderDataManager::ResetBaseSkinnedVertex();
        for (AnimatedGameObject& animatedGameObject : g_animatedGameObjects) {
            animatedGameObject.UpdateRenderItems();
            animatedGameObject.SubmitForSkinning();
            //animatedGameObject.DrawBones(WHITE);
            //animatedGameObject.DrawBoneTangentVectors();
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), animatedGameObject.GetRenderItems().begin(), animatedGameObject.GetRenderItems().end());
        }

        // Animated game objects
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Player* player = Game::GetLocalPlayerByIndex(i);
            AnimatedGameObject* viewWeapon = player->GetViewWeaponAnimatedGameObject();
            AnimatedGameObject* characterModel = player->GetCharacterModelAnimatedGameObject();
            viewWeapon->UpdateRenderItems();
            viewWeapon->SubmitForSkinning();
            characterModel->UpdateRenderItems();
            characterModel->SubmitForSkinning();
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), viewWeapon->GetRenderItems().begin(), viewWeapon->GetRenderItems().end());
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), characterModel->GetRenderItems().begin(), characterModel->GetRenderItems().end());
        }
    }

    void CreateGameObject() {
        g_gameObjects.emplace_back();
    }

    void CreateAnimatedGameObject() {
        g_animatedGameObjects.emplace_back();
    }

    AnimatedGameObject* GetAnimatedGameObjectByIndex(int32_t index) {
        if (index >= 0 && index < g_animatedGameObjects.size()) {
            return &g_animatedGameObjects[index];
        }
        else {
            return nullptr;
        }
    }

    GameObject* GetGameObjectByName(const std::string& name) {
        for (GameObject& gameObject : g_gameObjects) {
            if (gameObject.m_name == name) {
                return &gameObject;
            }
        }
        return nullptr;
    }

    GameObject* GetGameObjectByIndex(int32_t index) {
        if (index >= 0 && index < g_gameObjects.size()) {
            return &g_gameObjects[index];
        }
        else {
            return nullptr;
        }
    }

    Light* GetLightByIndex(int32_t index) {
        if (index >= 0 && index < g_lights.size()) {
            return &g_lights[index];
        }
        else {
            return nullptr;
        }
    }

    PickUp* GetPickUpByIndex(int32_t index) {
        if (index >= 0 && index < g_pickUps.size()) {
            return &g_pickUps[index];
        }
        else {
            return nullptr;
        }
    }
        
    Tree* GetTreeByIndex(int32_t index) {
        if (index >= 0 && index < g_trees.size()) {
            return &g_trees[index];
        }
        else {
            return nullptr;
        }
    }

    void NewCampainWorld() {
        ResetWorld();   
    }

    void LoadDeathMatchMap() {
        ResetWorld();
    }

    void LoadMap(const std::string& mapName) {
        MapCreateInfo* mapCreateInfo = MapManager::GetMapCreateInfoByName(mapName);

        // Handle failed map load
        if (!mapCreateInfo) {
            std::cout << "World::LoadMap() failed to load " << mapName << "\n";
            return;
        }

        g_currentMapName = mapName;

        // It loaded successfully so reset the world...
        ResetWorld();

        // and iterate over all the sector locations...
        for (auto& kv : mapCreateInfo->sectorLocations) {
            const ivecXZ& sectorLocation = kv.first;
            const std::string& sectorName = kv.second;

            // and if they are valid, then load them into the world
            SectorCreateInfo* sectorCreateInfo = SectorManager::GetSectorCreateInfoByName(sectorName);
            if (sectorCreateInfo) {
                SpawnOffset spawnOffset;
                spawnOffset.positionX = sectorLocation.x * SECTOR_SIZE_WORLD_SPACE;
                spawnOffset.positionZ = sectorLocation.z * SECTOR_SIZE_WORLD_SPACE;
                AddSectorAtLocation(*sectorCreateInfo, spawnOffset);
                std::cout << " - [" << sectorLocation.x << "][" << sectorLocation.z << "] " << sectorName << "\n";
            }
        }
        std::cout << "Loaded map: " << mapName << "\n";
    } 

    void LoadSingleSector(const std::string& sectorName) {
        SectorCreateInfo* sectorCreateInfo = SectorManager::GetSectorCreateInfoByName(sectorName);
        if (sectorCreateInfo) {
            LoadSingleSector(*sectorCreateInfo);
            std::cout << "Loaded sector: " << sectorName << "\n";
        }
        else {
            World::ResetWorld();
            std::cout << "Failed to load sector: " << sectorName << "\n";
        }
    }

    void LoadSingleSector(SectorCreateInfo& sectorCreateInfo) {
        ResetWorld();
        SpawnOffset spawnOffset;
        spawnOffset.positionX = 0;
        spawnOffset.positionZ = 0;
        AddSectorAtLocation(sectorCreateInfo, spawnOffset);
    }

    void AddSectorAtLocation(SectorCreateInfo& sectorCreateInfo, SpawnOffset spawnOffset) {
        for (LightCreateInfo& createInfo : sectorCreateInfo.lights) {
            AddLight(createInfo, spawnOffset);
        }
        for (GameObjectCreateInfo& createInfo : sectorCreateInfo.gameObjects) {
            AddGameObject(createInfo, spawnOffset);
        }
        for (PickUpCreateInfo& createInfo : sectorCreateInfo.pickUps) {
            AddPickUp(createInfo, spawnOffset);
        }
        for (TreeCreateInfo& createInfo : sectorCreateInfo.trees) {
            AddTree(createInfo, spawnOffset);
        }
    }

    void ResetWorld() {
        g_bulletCasings.clear();
        g_gameObjects.clear();
        g_lights.clear();
        g_pickUps.clear();
        g_trees.clear();
    }

    void AddBulletCasing(BulletCasingCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position.x += spawnOffset.positionX;
        createInfo.position.z += spawnOffset.positionZ;
        g_bulletCasings.push_back(BulletCasing(createInfo));
    }

    void AddGameObject(GameObjectCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position.x += spawnOffset.positionX;
        createInfo.position.z += spawnOffset.positionZ;
        g_gameObjects.push_back(GameObject(createInfo));
    }

    void AddLight(LightCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position.x += spawnOffset.positionX;
        createInfo.position.z += spawnOffset.positionZ;
        g_lights.push_back(Light(createInfo));
    }

    void AddPickUp(PickUpCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position.x += spawnOffset.positionX;
        createInfo.position.z += spawnOffset.positionZ;
        g_pickUps.push_back(PickUp(createInfo));
    }

    void AddTree(TreeCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position.x += spawnOffset.positionX;
        createInfo.position.z += spawnOffset.positionZ;
        g_trees.push_back(Tree(createInfo));
    }

    MapCreateInfo* GetCurrentMapCreateInfo() {
        return MapManager::GetMapCreateInfoByName(g_currentMapName); 
    }

    std::vector<AnimatedGameObject>& GetAnimatedGameObjects()   { return g_animatedGameObjects; }
    std::vector<BulletCasing>& GetBulletCasings()               { return g_bulletCasings; };
    std::vector<GameObject>& GetGameObjects()                   { return g_gameObjects; }
    std::vector<Light>& GetLights()                             { return g_lights; };
    std::vector<PickUp>& GetPickUps()                           { return g_pickUps; };
    std::vector<Tree>& GetTrees()                               { return g_trees; };
    std::vector<RenderItem>& GetRenderItems()                   { return g_renderItems; }
    std::vector<RenderItem>& GetRenderItemsBlended()            { return g_renderItemsBlended; }
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded()     { return g_renderItemsAlphaDiscarded; }
    std::vector<RenderItem>& GetRenderItemsHairTopLayer()       { return g_renderItemsHairTopLayer; }
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer()    { return g_renderItemsHairBottomLayer; }
    std::vector<RenderItem>& GetSkinnedRenderItems()            { return g_skinnedRenderItems; }
}