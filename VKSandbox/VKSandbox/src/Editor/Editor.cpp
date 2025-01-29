#include "Editor.h"
#include "Gizmo.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Audio.h"
#include "../Core/Debug.h"
#include "../Core/Scene.h"
#include "../Input/Input.h"

namespace Editor {

    bool g_isOpen = false;
    EditorObjectType g_selectedObjectType = EditorObjectType::NONE;
    uint16_t g_selectedObjectIndex = 0;

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
}