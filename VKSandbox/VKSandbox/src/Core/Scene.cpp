#include "Scene.h"

namespace Scene {
    std::vector<GameObject> g_gameObjects;
    std::vector<RenderItem> g_renderItems;
    std::vector<RenderItem> g_renderItemsBlended;
    std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    std::vector<RenderItem> g_renderItemsHairTopLayer;
    std::vector<RenderItem> g_renderItemsHairBottomLayer;

    void Init() {
        // Nothing as of yet
    }

    void Update(float deltaTime) {
        // Clear global render item vectors
        g_renderItems.clear();
        g_renderItemsBlended.clear();
        g_renderItemsAlphaDiscarded.clear();
        g_renderItemsHairTopLayer.clear();
        g_renderItemsHairBottomLayer.clear();

        // Update each GameObject and collect render items
        for (GameObject& gameObject : g_gameObjects) {
            gameObject.UpdateRenderItems();

            // Merge render items into global vectors
            g_renderItems.insert(g_renderItems.end(), gameObject.GetRenderItems().begin(), gameObject.GetRenderItems().end());
            g_renderItemsBlended.insert(g_renderItemsBlended.end(), gameObject.GetRenderItemsBlended().begin(), gameObject.GetRenderItemsBlended().end());
            g_renderItemsAlphaDiscarded.insert(g_renderItemsAlphaDiscarded.end(), gameObject.GetRenderItemsAlphaDiscarded().begin(), gameObject.GetRenderItemsAlphaDiscarded().end());
            g_renderItemsHairTopLayer.insert(g_renderItemsHairTopLayer.end(), gameObject.GetRenderItemsHairTopLayer().begin(), gameObject.GetRenderItemsHairTopLayer().end());
            g_renderItemsHairBottomLayer.insert(g_renderItemsHairBottomLayer.end(), gameObject.GetRenderItemsHairBottomLayer().begin(), gameObject.GetRenderItemsHairBottomLayer().end());
        }
    }

    void CreateGameObject() {
        g_gameObjects.emplace_back();
    }

    GameObject* GetGameObjectByName(const std::string& name) {
        for (GameObject& gameObject : g_gameObjects) {
            if (gameObject.m_name == name) {
                return &gameObject;
            }
        }
        return nullptr;
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
        mermaid->SetPosition(glm::vec3(3.5, -1.0f, 7.5f));
        mermaid->SetRotationY(3.14f * 1.7f);
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
        mermaid->m_mousePickIndex = 0;

        CreateGameObject();
        GameObject* scope = &g_gameObjects.back();
        scope->SetPosition(glm::vec3(2.76f, 0.0f, 6.2f));
        scope->SetModel("AKS74U_Scope4");
        scope->m_transform.scale = glm::vec3(0.01);
        scope->m_mousePickIndex = 1;
    }

    std::vector<RenderItem>& GetRenderItems() { return g_renderItems; }
    std::vector<RenderItem>& GetRenderItemsBlended() { return g_renderItemsBlended; }
    std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() { return g_renderItemsAlphaDiscarded; }
    std::vector<RenderItem>& GetRenderItemsHairTopLayer() { return g_renderItemsHairTopLayer; }
    std::vector<RenderItem>& GetRenderItemsHairBottomLayer() { return g_renderItemsHairBottomLayer; }
}
