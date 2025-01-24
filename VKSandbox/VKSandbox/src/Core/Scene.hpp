#pragma once
#include "HellTypes.h"
#include <vector>
#include "../AssetManagement/AssetManager.h"
#include "../Core/Camera.h"
#include "../Core/Debug.h"
#include "../Input/Input.h"
#include "../Types/GameObject.h"
#include "../Util.hpp"
#include "glm/gtx/intersect.hpp"

#include "../API/OpenGL/GL_renderer.h"

namespace Scene {

    inline std::vector<GameObject> g_gameObjects;
    inline std::vector<RenderItem> g_renderItems;
    inline std::vector<RenderItem> g_renderItemsBlended;
    inline std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    inline std::vector<RenderItem> g_renderItemsHairTopLayer;
    inline std::vector<RenderItem> g_renderItemsHairBottomLayer;

    inline glm::vec3 p1 = glm::vec3(0);
    inline glm::vec3 p2 = glm::vec3(0);


    inline glm::vec3 v0 = glm::vec3(0);
    inline glm::vec3 v1 = glm::vec3(0);
    inline glm::vec3 v2 = glm::vec3(0);
    inline glm::vec3 v3 = glm::vec3(0);

    inline void Init() {
        // nothing as of yet
    }

    inline void Update(float deltaTime) {
        // Clear global render item vectors
        g_renderItems.clear();
        g_renderItemsBlended.clear();
        g_renderItemsAlphaDiscarded.clear();
        g_renderItemsHairTopLayer.clear();
        g_renderItemsHairBottomLayer.clear();

        // Hack to translate the gizmo
        for (GameObject& gameObject : g_gameObjects) {         
            if (gameObject.m_model->GetName() == "Gizmo") {
                enum class Axis { X, Y, Z };
                static Axis g_axis = Axis::X;
                if (Input::KeyDown(HELL_KEY_X)) {
                    g_axis = Axis::X;
                }
                else if (Input::KeyDown(HELL_KEY_Y)) {
                    g_axis = Axis::Y;
                }
                else if (Input::KeyDown(HELL_KEY_Z)) {
                    g_axis = Axis::Z;
                }
                else {
                    break;
                }
                glm::mat4 projectionMatrix = Camera::GetProjectionMatrix();
                glm::mat4 viewMatrix = Camera::GetViewMatrix();
                glm::vec3 viewPos = Camera::GetViewPos();
                glm::vec3 gizmoPosition = gameObject.m_transform.position;
                glm::vec3 planeNormal = (g_axis == Axis::Y) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
                glm::vec3 rayOrigin = viewPos;
                glm::vec3 planeOrigin = glm::vec3(gizmoPosition);
                int mouseX = Input::GetMouseX();
                int mouseY = Input::GetMouseY();
                int windowWidth = BackEnd::GetCurrentWindowWidth();
                int windowHeight = BackEnd::GetCurrentWindowHeight();
                glm::vec3 rayDirection = Util::GetMouseRay(projectionMatrix, viewMatrix, windowWidth, windowHeight, mouseX, mouseY);
                float distanceToHit = 0;
                bool hitFound = glm::intersectRayPlane(rayOrigin, rayDirection, planeOrigin, planeNormal, distanceToHit);                
                if (hitFound) {
                    glm::vec3 hitPosition = rayOrigin + (rayDirection * distanceToHit);
                    switch (g_axis) {
                    case Axis::X: gameObject.m_transform.position.x = hitPosition.x; break;
                    case Axis::Y: gameObject.m_transform.position.y = hitPosition.y; break;
                    case Axis::Z: gameObject.m_transform.position.z = hitPosition.z; break;
                    }
                }
                // TODO: Once you this to the editor, make:
                // rayDirection = Editor::GetMouseRay();
            }
        }

        // Hack to scale the gizmo
        for (GameObject& gameObject : g_gameObjects) {
            if (gameObject.m_model->GetName() == "Gizmo") {
                float screenScaleFactor = 0.2f;
                float distance = glm::length(gameObject.m_transform.position - Camera::GetViewPos());
                float scalingFactor = distance * screenScaleFactor;
                gameObject.m_transform.scale = glm::vec3(scalingFactor);
            }
        }

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


    inline void CreateGameObject() {
        g_gameObjects.emplace_back();
    }

    inline GameObject* GetGameObjectByName(const std::string& name) {
        for (GameObject& gameObject : g_gameObjects) {
            if (gameObject.m_name == name) {
                return &gameObject;
            }
        }
        return nullptr;
    }

    inline void SetMaterials() {
        GameObject* room = GetGameObjectByName("Room");
        if (room) {
            room->SetMeshMaterialByMeshName("PlatformSide", "BathroomFloor");
            room->SetMeshMaterialByMeshName("PlatformTop", "BathroomFloor");
            room->SetMeshMaterialByMeshName("Floor", "BathroomFloor");
            room->SetMeshMaterialByMeshName("Ceiling", "Ceiling2");
            room->SetMeshMaterialByMeshName("WallZPos", "BathroomWall");
            room->SetMeshMaterialByMeshName("WallZNeg", "BathroomWall");
            room->SetMeshMaterialByMeshName("WallXPos", "BathroomWall");
            room->SetMeshMaterialByMeshName("WallXNeg", "BathroomWall");
        }

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

    inline void CreateGameObjects() {
        //CreateGameObject();
        //GameObject* room = &g_gameObjects[g_gameObjects.size() - 1];
        //room->SetModel("Room");
        //room->SetMeshMaterialByMeshName("PlatformSide", "BathroomFloor");
        //room->SetMeshMaterialByMeshName("PlatformTop", "BathroomFloor");
        //room->SetMeshMaterialByMeshName("Floor", "BathroomFloor");
        //room->SetMeshMaterialByMeshName("Ceiling", "Ceiling2");
        //room->SetMeshMaterialByMeshName("WallZPos", "BathroomWall");
        //room->SetMeshMaterialByMeshName("WallZNeg", "BathroomWall");
        //room->SetMeshMaterialByMeshName("WallXPos", "BathroomWall");
        //room->SetMeshMaterialByMeshName("WallXNeg", "BathroomWall");

      // CreateGameObject();
      // GameObject* gizmo = &g_gameObjects[g_gameObjects.size() - 1];
      // gizmo->SetPosition(glm::vec3(3.8, -1.0f, 5.25f));
      // gizmo->SetModel("Gizmo");
      // gizmo->PrintMeshNames();

        //CreateGameObject();
        //GameObject* scope = &g_gameObjects[g_gameObjects.size() - 1];
        //scope->SetPosition(glm::vec3(0, 0, 0));
        //scope->SetModel("AKS74U_Scope4");
        //scope->m_transform.scale = glm::vec3(0.01);
        //scope->PrintMeshNames();
        //
        //CreateGameObject();
        //GameObject* scope2 = &g_gameObjects[g_gameObjects.size() - 1];
        //scope2->SetPosition(glm::vec3(0, 0, 1));
        //scope2->SetModel("AKS74U_Scope");
        //scope2->m_transform.scale = glm::vec3(0.01);
        //scope2->PrintMeshNames();

        CreateGameObject();
        GameObject* mermaid = &g_gameObjects[g_gameObjects.size() - 1];
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
    }

    inline std::vector<RenderItem>& GetRenderItems() { return g_renderItems; }
    inline std::vector<RenderItem>& GetRenderItemsBlended() { return g_renderItemsBlended; }
    inline std::vector<RenderItem>& GetRenderItemsAlphaDiscarded() { return g_renderItemsAlphaDiscarded; }
    inline std::vector<RenderItem>& GetRenderItemsHairTopLayer() { return g_renderItemsHairTopLayer; }
    inline std::vector<RenderItem>& GetRenderItemsHairBottomLayer() { return g_renderItemsHairBottomLayer; }
}