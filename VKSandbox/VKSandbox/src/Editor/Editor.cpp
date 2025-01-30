#include "Editor.h"
#include "Gizmo.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Audio.h"
#include "../Core/Debug.h"
#include "../Core/Scene.h"
#include "../Input/Input.h"
#include "../Types/Camera.h"

namespace Editor {

    bool g_isOpen = false;
    float g_splitX = 0.4f;
    float g_splitY = 0.4f;
    EditorObjectType g_selectedObjectType = EditorObjectType::NONE;
    uint16_t g_selectedObjectIndex = 0;
    Camera g_cameras[4];
    bool g_isOrthographic[4];

    void Init() {

        // Top left
        g_cameras[0].SetPosition(glm::vec3(0, 2.5f, 0.0f));
        g_cameras[0].SetEulerRotation(glm::vec3(HELL_PI * -0.5f, HELL_PI * -0.5f, 0.0f));
        g_isOrthographic[0] = true;

        // Top right
        g_cameras[1].SetPosition(glm::vec3(0.0f, 0.8f, 2.5f));
        g_cameras[1].SetEulerRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        g_isOrthographic[1] = true;

        // Bottom left
        g_cameras[2].SetPosition(glm::vec3(-2.5f, 0.8f, 0.0f));
        g_cameras[2].SetEulerRotation(glm::vec3(0.0f, HELL_PI * -0.5f, 0.0f));
        g_isOrthographic[2] = true;

        // Bottom right
        g_cameras[3].SetPosition(glm::vec3(-2.09f, 1.45f, 0.68f));
        g_cameras[3].SetEulerRotation(glm::vec3(-0.30f, -1.32f, 0.0f));
        g_isOrthographic[3] = false;

        //AddLocalPlayer(glm::vec3(3.18f, 0.55f, 6.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        //AddLocalPlayer(glm::vec3(0.0f, 1.2f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
        //AddLocalPlayer(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(-HELL_PI / 2, -HELL_PI / 2, 0.0f));
        //AddLocalPlayer(glm::vec3(-2.5f, 1.2f, 0.0f), glm::vec3(0.0f, -HELL_PI / 2, 0.0f));
    }

    void Update() {

        // Object Selection
        uint16_t mousePickType = BackEnd::GetMousePickR();
        uint16_t mousePickIndex = BackEnd::GetMousePickG();
        if (Input::LeftMousePressed() && !Gizmo::HasHover()) {
            Gizmo::SetPosition(Scene::g_gameObjects[mousePickIndex].m_transform.position);
            g_selectedObjectType = static_cast<EditorObjectType>(mousePickType);
            g_selectedObjectIndex = mousePickIndex;
        }

        // Translate the selected object
        if (g_selectedObjectType == EditorObjectType::GAME_OBJECT) {
            Scene::g_gameObjects[g_selectedObjectIndex].m_transform.position = Gizmo::GetPosition();
        }

        UpdateDebug();
    }

    void Open() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::ShowCursor();
        Input::SetCursorPosition(BackEnd::GetCurrentWindowWidth() / 2, BackEnd::GetCurrentWindowHeight() / 2);
        g_isOpen = true;
    }

    void Close() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
        Input::DisableCursor();
        g_isOpen = false;
    }

    void ToggleOpenState() {
        if (g_isOpen) {
            Close();
        }
        else {
            Open();
        }
    }

    bool IsOpen() {
        return g_isOpen;
    }

    EditorObjectType GetSelectedObjectType() {
        return g_selectedObjectType;
    }

    std::string EditorObjectTypeToString(const EditorObjectType& type) {
        switch (type) {
        case EditorObjectType::NONE:        return "None";
        case EditorObjectType::GAME_OBJECT: return "Game Object";
        case EditorObjectType::DOOR:        return "Door";
        case EditorObjectType::WINDOW:      return "Window";
        case EditorObjectType::CSG:         return "CSG";
        default:                            return "Unknown";
        }
    }

    bool IsViewportOrthographic(uint32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_isOrthographic[viewportIndex];
        }
        else {
            std::cout << "Game::GetCameraByIndex(int index) failed. " << viewportIndex << " out of range of editor viewport count 4\n";
            return false;
        }
    }

    Camera* GetCameraByIndex(uint32_t index) {
        if (index >= 0 && index < 4) {
            return &g_cameras[index];
        }
        else {
            std::cout << "Game::GetCameraByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
            return nullptr;
        }
    }

    float GetSplitX() {
        return g_splitX;
    }

    float GetSplitY() {
        return g_splitY;
    }
}