#include "Scene.h"
#include "HellDefines.h"
#include "Core/Game.h"
#include "Core/Map.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"

namespace Scene {
    std::vector<AnimatedGameObject> g_animatedGameObjects;
    std::vector<BulletCasing> g_bulletCasings;
    std::vector<GameObject> g_gameObjects;
    std::vector<Light> g_lights;

    std::vector<RenderItem> g_renderItems;
    std::vector<RenderItem> g_renderItemsBlended;
    std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    std::vector<RenderItem> g_renderItemsHairTopLayer;
    std::vector<RenderItem> g_renderItemsHairBottomLayer;
    std::vector<RenderItem> g_skinnedRenderItems;

    void UpdateObjects();
    void UpdateRenderItems();

    void Init() {

        float magic = 2.7192f;

        // Hardcode some test lights
        LightCreateInfo createInfo;
        createInfo.position = glm::vec3(16.0f, -5.0f + magic, 16.0f);
        createInfo.type = Util::LightTypeToString(LightType::LAMP_POST);
        createInfo.strength = 5.0f * 0.85;
        createInfo.radius = 7 * 0.8;
        AddLight(createInfo);

        createInfo.position = glm::vec3(22.87f, -5.0f + magic, 21.13f);
        createInfo.type = Util::LightTypeToString(LightType::LAMP_POST);
        createInfo.strength = 5.0f * 0.85;
        createInfo.radius = 7 * 0.8;
        AddLight(createInfo);

        createInfo.position = glm::vec3(6.2f, -5.0f + magic, 34.13f);
        createInfo.type = Util::LightTypeToString(LightType::LAMP_POST);
        createInfo.strength = 5.0f * 0.85;
        createInfo.radius = 7 * 0.8;
        AddLight(createInfo);

    }

    void LoadHeightMap() {

    }

    void Update(float deltaTime) {
        UpdateObjects();
        UpdateRenderItems();

        if (Input::KeyPressed(HELL_KEY_O)) {
            //SaveMap("res/maps/house0.json");
        }
        if (Input::KeyPressed(HELL_KEY_O)) {
            LoadHeightMap();
        }
    }

    void UpdateObjects() {
        float deltaTime = Game::GetDeltaTime();

        for (GameObject& gameObject : g_gameObjects) {
            // TODO: gameObject.Update();
        }
        for (AnimatedGameObject& animatedGameObject : g_animatedGameObjects) {
            animatedGameObject.Update(deltaTime);
        }
    }

    void UpdateRenderItems() {
        // Clear global render item vectors
        g_renderItems.clear();
        g_skinnedRenderItems.clear();
        g_renderItemsBlended.clear();
        g_renderItemsAlphaDiscarded.clear();
        g_renderItemsHairTopLayer.clear();
        g_renderItemsHairBottomLayer.clear();

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

        for (BulletCasing& casing : Scene::g_bulletCasings) {
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
    
    void Scene::CreateAnimatedGameObject() {
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

    void SetMaterials() {
        GameObject* mermaid = GetGameObjectByName("Mermaid");
        if (mermaid) {
            mermaid->SetMeshMaterialByMeshName("Rock", "Rock");
            mermaid->SetMeshMaterialByMeshName("BoobTube", "BoobTube");
            mermaid->SetMeshMaterialByMeshName("Face", "MermaidFace");
            mermaid->SetMeshMaterialByMeshName("Body", "MermaidBody");
            mermaid->SetMeshMaterialByMeshName("Arms", "MermaidArms");
            mermaid->SetMeshMaterialByMeshName("HairInner", "MermaidHair");
            mermaid->SetMeshMaterialByMeshName("HairOutta", "MermaidHair");
            mermaid->SetMeshMaterialByMeshName("HairScalp", "MermaidScalp");
            mermaid->SetMeshMaterialByMeshName("EyeLeft", "MermaidEye");
            mermaid->SetMeshMaterialByMeshName("EyeRight", "MermaidEye");
            mermaid->SetMeshMaterialByMeshName("Tail", "MermaidTail");
            mermaid->SetMeshMaterialByMeshName("TailFin", "MermaidTail");
            mermaid->SetMeshMaterialByMeshName("EyelashUpper_HP", "MermaidLashes");
            mermaid->SetMeshMaterialByMeshName("EyelashLower_HP", "MermaidLashes");
            mermaid->SetMeshMaterialByMeshName("Nails", "Nails");
        }
    }

    void CreateGameObjects() {

        CreateGameObject();
        GameObject* mermaid = &g_gameObjects.back();
        mermaid->SetPosition(glm::vec3(19.64f, -4.62f, 17.33f));
        mermaid->SetRotationY(0.0f);
        mermaid->SetModel("Mermaid");
        mermaid->SetMeshMaterialByMeshName("Rock", "Rock");
        mermaid->SetMeshMaterialByMeshName("BoobTube", "BoobTube");
        mermaid->SetMeshMaterialByMeshName("Face", "MermaidFace");
        mermaid->SetMeshMaterialByMeshName("Body", "MermaidBody");
        mermaid->SetMeshMaterialByMeshName("Arms", "MermaidArms");
        mermaid->SetMeshMaterialByMeshName("HairInner", "MermaidHair");
        mermaid->SetMeshMaterialByMeshName("HairOutta", "MermaidHair");
        mermaid->SetMeshMaterialByMeshName("HairScalp", "MermaidScalp");
        mermaid->SetMeshMaterialByMeshName("EyeLeft", "MermaidEye");
        mermaid->SetMeshMaterialByMeshName("EyeRight", "MermaidEye");
        mermaid->SetMeshMaterialByMeshName("Tail", "MermaidTail");
        mermaid->SetMeshMaterialByMeshName("TailFin", "MermaidTail");
        mermaid->SetMeshMaterialByMeshName("EyelashUpper_HP", "MermaidLashes");
        mermaid->SetMeshMaterialByMeshName("EyelashLower_HP", "MermaidLashes");
        mermaid->SetMeshMaterialByMeshName("Nails", "Nails");
        mermaid->SetMeshBlendingMode("EyelashUpper_HP", BlendingMode::BLENDED);
        mermaid->SetMeshBlendingMode("EyelashLower_HP", BlendingMode::BLENDED);
        mermaid->SetMeshBlendingMode("HairScalp", BlendingMode::BLENDED);
        mermaid->SetMeshBlendingMode("HairOutta", BlendingMode::HAIR_TOP_LAYER);
        mermaid->SetMeshBlendingMode("HairInner", BlendingMode::HAIR_UNDER_LAYER);
        mermaid->SetName("Mermaid");

      //for (int i = 0; i < 500; i++) {
      //    float x = Util::RandomFloat(-30.0f, 30.0f);
      //    float y = Util::RandomFloat(3.0f, 25);
      //    float z = Util::RandomFloat(-30.0f, 30.0f);
      //    CreateGameObject();
      //    GameObject* cube = &g_gameObjects.back();
      //    cube->SetPosition(glm::vec3(x, y, z));
      //    cube->SetRotation(glm::vec3(Util::RandomFloat(-HELL_PI, HELL_PI), Util::RandomFloat(-HELL_PI, HELL_PI), Util::RandomFloat(-HELL_PI, HELL_PI)));
      //    cube->SetModel("Cube");
      //    cube->SetMeshMaterials("MermaidTail");
      //}



        CreateGameObject();
        GameObject* bench = &g_gameObjects.back();
        bench->SetPosition(glm::vec3(17.24f, -5.0f, 16.53f));
        bench->SetRotationY(-0.4f);
        bench->SetModel("Bench");
        bench->SetMeshMaterials("LampPost");

        CreateGameObject();
        GameObject* tree = &g_gameObjects.back();
        tree->SetPosition(glm::vec3(29.31f, -4.87f, 28.86f));
        tree->SetRotationY(-0.4f);
        tree->SetModel("TreeLarge_0");
        tree->SetMeshMaterials("TreeLarge_0");

        CreateGameObject();
        GameObject* tree2 = &g_gameObjects.back();
        tree2->SetPosition(glm::vec3(29.31f, -4.87f, 27.86f));
        tree2->SetRotationY(-0.4f);
        tree2->SetModel("TreeLarge_1");
        tree2->SetMeshMaterials("TreeLarge_0");

        CreateGameObject();
        GameObject* tree3 = &g_gameObjects.back();
        tree3->SetPosition(glm::vec3(29.31f, -4.87f, 25.86f));
        tree3->SetRotationY(-0.4f);
        tree3->SetModel("TreeLarge_2");
        tree3->SetMeshMaterials("TreeLarge_0");

     //  CreateGameObject();
     //  GameObject* bench2 = &g_gameObjects.back();
     //  bench2->SetPosition(glm::vec3(17.24f, -5.0f, 16.53f));
     //  bench2->SetRotationY(-0.4f);
     //  bench2->SetModel("LampPost");
     //  bench2->SetMeshMaterials("LampPost");



        return;

        CreateAnimatedGameObject();
        AnimatedGameObject& object = g_animatedGameObjects.back();
        object.SetPlayerIndex(1);
        object.SetSkinnedModel("Shark");
        object.SetName("222");
        object.SetAnimationModeToBindPose();
        object.SetAllMeshMaterials("Shark");
        object.SetPosition(glm::vec3(3.8f, -0.5, 11.3f));
        object.SetScale(0.01);
        object.PlayAndLoopAnimation("Shark_Attack_Left_Quick");

      

        CreateAnimatedGameObject();
        AnimatedGameObject& object2 = g_animatedGameObjects.back();
        object2.SetPlayerIndex(1);
        object2.SetSkinnedModel("Shark");
        object2.SetName("222");
        object2.SetAnimationModeToBindPose();
        object2.SetAllMeshMaterials("Shark");
        object2.SetPosition(glm::vec3(7.8f, -0.5, 11.3f));
        object2.SetScale(0.01);
        object2.SetAnimationModeToBindPose();
        return;
        CreateAnimatedGameObject();
        AnimatedGameObject& object3 = g_animatedGameObjects.back();
        object3.SetPlayerIndex(1);
        object3.SetSkinnedModel("Glock");
        object3.SetName("222");
        object3.SetAnimationModeToBindPose();
        object3.SetAllMeshMaterials("Doberman");
        object3.SetPosition(glm::vec3(0.8f, -0.5, 11.3f));
        object3.SetScale(0.01);
        object3.PlayAndLoopAnimation("Glock_ReloadEmpty");
        //object3.SetAnimationModeToBindPose();
    }

    void AddLight(LightCreateInfo createInfo) {
        g_lights.push_back(Light(createInfo));
    }

    void AddBulletCasing(BulletCasingCreateInfo createInfo) {
        g_bulletCasings.push_back(BulletCasing(createInfo));
    }

    void SaveMap(const std::string& filepath) {
        MapData mapData;
        for (Light& light : g_lights) {
            mapData.lights.push_back(light.GetCreateInfo());
;       }
        Map::Save(filepath, mapData);
        std::cout << "Saved: " << filepath << "\n";
    }

    std::vector<AnimatedGameObject>& GetAnimatedGameObjects()   { return g_animatedGameObjects; }
    std::vector<BulletCasing>& GetBulletCasings()               { return g_bulletCasings; };
    std::vector<GameObject>& GetGameObjects()                   { return g_gameObjects; }
    std::vector<Light>& GetLights()                             { return g_lights; };
    std::vector<RenderItem>& GetRenderItems()                   { return g_renderItems; }
    std::vector<RenderItem>& GetRenderItemsBlended()            { return g_renderItemsBlended; }
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded()     { return g_renderItemsAlphaDiscarded; }
    std::vector<RenderItem>& GetRenderItemsHairTopLayer()       { return g_renderItemsHairTopLayer; }
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer()    { return g_renderItemsHairBottomLayer; }
    std::vector<RenderItem>& GetSkinnedRenderItems()            { return g_skinnedRenderItems; }
}
