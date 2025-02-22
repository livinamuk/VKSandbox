#include "World.h"

#define WORLD_SECTOR_WIDTH 6
#define WORLD_SECTOR_HEIGHT 20

#include "Util.h"
#include "Core/Game.h"

#include "HellDefines.h"
#include "Core/Game.h"
#include "Core/Map.h"
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


namespace World {

    Sector g_sectors[WORLD_SECTOR_WIDTH][WORLD_SECTOR_HEIGHT];
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


    void CreateHardCodedTestSector() {

        ResetWorld();

        float magic = 2.7192f;

        SectorData sectorData;

        LightCreateInfo createInfo;
        createInfo.position = glm::vec3(16.0f, -5.0f + magic, 16.0f);
        createInfo.type = Util::LightTypeToString(LightType::LAMP_POST);
        createInfo.strength = 5.0f * 0.85;
        createInfo.radius = 7 * 0.8;
        sectorData.lights.push_back(createInfo);
        
        AddLight(createInfo); // remove me when you can
        
        createInfo.position = glm::vec3(22.87f, -5.0f + magic, 21.13f);
        createInfo.type = Util::LightTypeToString(LightType::LAMP_POST);
        createInfo.strength = 5.0f * 0.85;
        createInfo.radius = 7 * 0.8;
        sectorData.lights.push_back(createInfo);
        
        AddLight(createInfo); // remove me when you can
        
        createInfo.position = glm::vec3(6.2f, -5.0f + magic, 34.13f);
        createInfo.type = Util::LightTypeToString(LightType::LAMP_POST);
        createInfo.strength = 5.0f * 0.85;
        createInfo.radius = 7 * 0.8;
        sectorData.lights.push_back(createInfo);
        
        AddLight(createInfo); // remove me when you can


         
        PickUpCreateInfo pickUpCreateInfo;
        pickUpCreateInfo.position = glm::vec3(16.10f, -4.95f, 16.97f);
        pickUpCreateInfo.rotation = glm::vec3(0.0f);
        pickUpCreateInfo.pickUpType = Util::PickUpTypeToString(PickUpType::SHOTGUN_AMMO_BUCKSHOT);
        sectorData.pickUps.push_back(pickUpCreateInfo);
        AddPickUp(pickUpCreateInfo);

        pickUpCreateInfo;
        pickUpCreateInfo.position = glm::vec3(16.50f, -4.95f, 16.97f);
        pickUpCreateInfo.rotation = glm::vec3(0.0f);
        pickUpCreateInfo.pickUpType = Util::PickUpTypeToString(PickUpType::SHOTGUN_AMMO_SLUG);
        sectorData.pickUps.push_back(pickUpCreateInfo);
        AddPickUp(pickUpCreateInfo);

        // Bench
        GameObjectCreateInfo gameObjectCreateInfo;
        gameObjectCreateInfo.position = glm::vec3(17.24f, -5.0f, 16.53f);
        gameObjectCreateInfo.rotation = glm::vec3(0.0f, -0.4f, 0.0f);
        gameObjectCreateInfo.scale = glm::vec3(1.0f);
        gameObjectCreateInfo.modelName = "Bench";
        gameObjectCreateInfo.baseMaterial = "LampPost";
        sectorData.gameObjects.push_back(gameObjectCreateInfo);
        AddGameObject(gameObjectCreateInfo);

        // Mermaid
        gameObjectCreateInfo;
        gameObjectCreateInfo.baseMaterial = "";
        gameObjectCreateInfo.position = glm::vec3(19.64f, -4.62f, 17.33f);
        gameObjectCreateInfo.rotation = glm::vec3(0.0f);
        gameObjectCreateInfo.scale = glm::vec3(1.0f);
        gameObjectCreateInfo.modelName = "Mermaid";
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "Rock", "Rock" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "BoobTube", "BoobTube" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "Face", "MermaidFace" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "Body", "MermaidBody" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "Arms", "MermaidArms" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "HairInner", "MermaidHair" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "HairOutta", "MermaidHair" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "HairScalp", "MermaidScalp" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "EyeLeft", "MermaidEye" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "EyeRight", "MermaidEye" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "Tail", "MermaidTail" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "TailFin", "MermaidTail" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "EyelashUpper_HP", "MermaidLashes" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "EyelashLower_HP", "MermaidLashes" });
        gameObjectCreateInfo.meshMaterialInfo.push_back({ "Nails", "Nails" });
        gameObjectCreateInfo.meshBlendingInfo.push_back({ "EyelashUpper_HP", BlendingMode::BLENDED });
        gameObjectCreateInfo.meshBlendingInfo.push_back({ "EyelashLower_HP", BlendingMode::BLENDED });
        gameObjectCreateInfo.meshBlendingInfo.push_back({ "HairScalp", BlendingMode::BLENDED });
        gameObjectCreateInfo.meshBlendingInfo.push_back({ "HairOutta", BlendingMode::HAIR_TOP_LAYER });
        gameObjectCreateInfo.meshBlendingInfo.push_back({ "HairInner", BlendingMode::HAIR_UNDER_LAYER });
        sectorData.gameObjects.push_back(gameObjectCreateInfo);
        AddGameObject(gameObjectCreateInfo);


        TreeCreateInfo treeCreateInfo;
        treeCreateInfo.position = glm::vec3(29.31f, -4.87f, 28.86f);
        treeCreateInfo.type = (int)TreeType::TREE_LARGE_0;
        sectorData.trees.push_back(treeCreateInfo);
        AddTree(treeCreateInfo);

        treeCreateInfo;
        treeCreateInfo.position = glm::vec3(31.31f, -4.87f, 28.86f);
        treeCreateInfo.type = (int)TreeType::TREE_LARGE_1;
        sectorData.trees.push_back(treeCreateInfo);
        AddTree(treeCreateInfo);

        treeCreateInfo;
        treeCreateInfo.position = glm::vec3(33.31f, -4.87f, 28.86f);
        treeCreateInfo.type = (int)TreeType::TREE_LARGE_2;
        sectorData.trees.push_back(treeCreateInfo);
        AddTree(treeCreateInfo);

        SaveSectorData("res/sectors/TestSector.json", sectorData);
    }



    SectorData LoadSectorData(const std::string& filepath) {      
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();

        SectorData sectorData;
        nlohmann::json json = nlohmann::json::parse(buffer.str());

        // Load lights
        for (auto& jsonObject : json["Lights"]) {
            LightCreateInfo& createInfo = sectorData.lights.emplace_back();
            createInfo.position = JSON::JsonToVec3(jsonObject["position"]);
            createInfo.color = JSON::JsonToVec3(jsonObject["color"]);
            createInfo.radius = jsonObject["radius"];
            createInfo.strength = jsonObject["strength"];
            createInfo.type = jsonObject["type"];
        }

        // Load Game Objects
        for (auto& jsonObject : json["GameObjects"]) {
            GameObjectCreateInfo& createInfo = sectorData.gameObjects.emplace_back();
            createInfo.position = JSON::JsonToVec3(jsonObject["position"]);
            createInfo.rotation = JSON::JsonToVec3(jsonObject["rotation"]);
            createInfo.scale = JSON::JsonToVec3(jsonObject["scale"]);
            createInfo.modelName = jsonObject["modelName"];
            createInfo.baseMaterial = jsonObject["baseMaterial"];

            std::vector<std::string> meshMaterialInfoStrings = jsonObject["meshMaterials"];
            for (std::string& str : meshMaterialInfoStrings) {
                createInfo.meshMaterialInfo.push_back(Util::StringToMeshMaterialInfo(str));
            }

            std::vector<std::string> meshBlendingModeStrings = jsonObject["meshBlendingModes"];
            for (std::string& str : meshBlendingModeStrings) {
                createInfo.meshBlendingInfo.push_back(Util::StringToMeshBlendingMode(str));
            }
        }
        return sectorData;
    }



    void SaveSectorData(const std::string& filepath, SectorData& sectorData) {

        nlohmann::json json;
        json["Lights"] = nlohmann::json::array();
        json["GameObjects"] = nlohmann::json::array();

        for (const LightCreateInfo& createInfo : sectorData.lights) {
            json["Lights"].push_back(nlohmann::json{
                { "position", JSON::Vec3ToJson(createInfo.position)},
                { "color", JSON::Vec3ToJson(createInfo.color) },
                { "type", createInfo.type },
                { "radius", createInfo.radius },
                { "strength", createInfo.strength }
            });
        }

        for (const TreeCreateInfo& createInfo : sectorData.trees) {
            json["Trees"].push_back(nlohmann::json{
                { "position", JSON::Vec3ToJson(createInfo.position)},
                { "rotation", JSON::Vec3ToJson(createInfo.rotation) },
                { "scale", JSON::Vec3ToJson(createInfo.scale) },
                { "type", (int)createInfo.type }
            });
        }

        for (const PickUpCreateInfo& createInfo : sectorData.pickUps) {
            json["PickUps"].push_back(nlohmann::json{
                { "position", JSON::Vec3ToJson(createInfo.position)},
                { "rotation", JSON::Vec3ToJson(createInfo.rotation) },
                { "type", createInfo.pickUpType }
            });
        }

        for (const GameObjectCreateInfo& createInfo : sectorData.gameObjects) {

            std::vector<std::string> meshMaterialInfoList;
            for (const MeshMaterialInfo& meshMaterialInfo : createInfo.meshMaterialInfo) {
                meshMaterialInfoList.push_back(Util::MeshMaterialInfoToString(meshMaterialInfo));
            }

            std::vector<std::string> meshBlendingInfoList;
            for (const MeshBlendingInfo& meshBlendingInfo : createInfo.meshBlendingInfo) {
                meshBlendingInfoList.push_back(Util::MeshBlendingInfoToString(meshBlendingInfo));
            }

            json["GameObjects"].push_back(nlohmann::json{
                { "position", JSON::Vec3ToJson(createInfo.position)},
                { "rotation", JSON::Vec3ToJson(createInfo.rotation) },
                { "scale", JSON::Vec3ToJson(createInfo.scale) },
                { "modelName", createInfo.modelName },
                { "baseMaterial", createInfo.baseMaterial },
                { "meshMaterials", meshMaterialInfoList },
                { "meshBlendingModes", meshBlendingInfoList },
            });
        }

        JSON::SaveToFile(json, filepath);
    }
















    void Update(float deltaTime) {

        

        for (GameObject& gameObject : g_gameObjects) {

            if (Input::KeyPressed(HELL_KEY_DELETE)) {

                std::cout << gameObject.m_model->GetName() << "\n";
                for (int i = 0; i < gameObject.m_model->GetMeshCount(); i++) {
                    Mesh* mesh = AssetManager::GetMeshByIndex(gameObject.m_model->GetMeshIndices()[i]);
                    Material* material = AssetManager::GetMaterialByIndex(gameObject.m_meshMaterialIndices[i]);
                    std::cout << i << ": " << mesh->GetName() << " " << material->m_name << "\n";
                }
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

        for (PickUp& pickUp : g_pickUps) {
            pickUp.Update(deltaTime);
            g_renderItems.insert(g_renderItems.end(), pickUp.GetRenderItems().begin(), pickUp.GetRenderItems().end());
        }

        for (Tree& tree : g_trees) {
            tree.Update(deltaTime);
            g_renderItems.insert(g_renderItems.end(), tree.GetRenderItems().begin(), tree.GetRenderItems().end());
        }

        // Update each GameObject and collect render items
        int mousePickIndex = 0;
        for (GameObject& gameObject : g_gameObjects) {
            gameObject.UpdateRenderItems();
            gameObject.SetMousePickIndex(mousePickIndex++);
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
            //animatedGameObject.DrawBones(WHITE);
            //animatedGameObject.DrawBoneTangentVectors();
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), animatedGameObject.GetRenderItems().begin(), animatedGameObject.GetRenderItems().end());
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

    void NewCampainWorld() {
        ResetWorld();   
    }

    void LoadDeathMatchMap() {
        ResetWorld();
    }

    void LoadSingleSector(SectorData& sectorData) {
        ResetWorld();
        for (LightCreateInfo& createInfo : sectorData.lights) {
            AddLight(createInfo);
        }
        for (GameObjectCreateInfo& createInfo : sectorData.gameObjects) {
            AddGameObject(createInfo);
        }        
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

    std::vector<AnimatedGameObject>& GetAnimatedGameObjects() { return g_animatedGameObjects; }
    std::vector<BulletCasing>& GetBulletCasings() { return g_bulletCasings; };
    std::vector<GameObject>& GetGameObjects() { return g_gameObjects; }
    std::vector<Light>& GetLights() { return g_lights; };
    std::vector<RenderItem>& GetRenderItems() { return g_renderItems; }
    std::vector<RenderItem>& GetRenderItemsBlended() { return g_renderItemsBlended; }
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() { return g_renderItemsAlphaDiscarded; }
    std::vector<RenderItem>& GetRenderItemsHairTopLayer() { return g_renderItemsHairTopLayer; }
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer() { return g_renderItemsHairBottomLayer; }
    std::vector<RenderItem>& GetSkinnedRenderItems() { return g_skinnedRenderItems; }
}