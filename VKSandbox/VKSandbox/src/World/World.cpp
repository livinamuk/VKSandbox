#include "World.h"

#define WORLD_SECTOR_WIDTH 6
#define WORLD_SECTOR_HEIGHT 20

#include "Util.h"
#include "Core/Audio.h"
#include "Core/Game.h"

#include "HellDefines.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"

#include "HellTypes.h"
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "CreateInfo.h"
#include "Util.h"

#include "Core/JSON.h"
#include <fstream>
#include <string>
#include <iostream>


#include "API/OpenGL/Renderer/GL_renderer.h"

namespace World {

    //Sector g_sectors[WORLD_SECTOR_WIDTH][WORLD_SECTOR_HEIGHT];
    SectorCreateInfo g_editorSectorCreateInfo;

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

    void ResetWorld();
    void AddLight(LightCreateInfo createInfo);
    void AddTree(TreeCreateInfo createInfo);

    SectorCreateInfo& GetEditorSectorCreateInfo() {
        return g_editorSectorCreateInfo;
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

        mousePickIndex = 0;
        for (Tree& tree : g_trees) {
            tree.SetMousePickIndex(mousePickIndex++);
            tree.Update(deltaTime);
            g_renderItems.insert(g_renderItems.end(), tree.GetRenderItems().begin(), tree.GetRenderItems().end());
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

    void LoadSingleSector(SectorCreateInfo& sectorCreateInfo) {
        ResetWorld();
        for (LightCreateInfo& createInfo : sectorCreateInfo.lights) {
            AddLight(createInfo);
        }
        for (GameObjectCreateInfo& createInfo : sectorCreateInfo.gameObjects) {
            AddGameObject(createInfo);
        }
        for (PickUpCreateInfo& createInfo : sectorCreateInfo.pickUps) {
            AddPickUp(createInfo);
        }
        for (TreeCreateInfo& createInfo : sectorCreateInfo.trees) {
            AddTree(createInfo);
        }
        g_editorSectorCreateInfo = sectorCreateInfo;
    }

    void ResetWorld() {
        g_bulletCasings.clear();
        g_gameObjects.clear();
        g_lights.clear();
        g_pickUps.clear();
        g_trees.clear();

        //for (int x = 0; x < WORLD_SECTOR_WIDTH; x++) {
        //    for (int y = 0; y < WORLD_SECTOR_HEIGHT; y++) {
        //        g_sectors[x][y].ResetData();
        //    }
        //}
    }

    void AddBulletCasing(BulletCasingCreateInfo createInfo) {
        g_bulletCasings.push_back(BulletCasing(createInfo));
    }

    void AddGameObject(GameObjectCreateInfo createInfo) {
        g_gameObjects.push_back(GameObject(createInfo));
    }

    void AddLight(LightCreateInfo createInfo) {
        g_lights.push_back(Light(createInfo));
    }

    void AddPickUp(PickUpCreateInfo createInfo) {
        g_pickUps.push_back(PickUp(createInfo));
    }

    void AddTree(TreeCreateInfo createInfo) {
        g_trees.push_back(Tree(createInfo));
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