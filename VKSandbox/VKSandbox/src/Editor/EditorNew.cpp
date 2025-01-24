#include "EditorNew.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Audio.h"
#include "../Input/Input.h"

namespace EditorNew {

    bool g_isOpen = false;
    
    void Update() {
        // Bail if editor is closed
        if (!IsOpen()) {
            return;
        }
        // Mouse picking
        if (BackEnd::GetAPI() == API::OPENGL) {
            float textureWidth = 1920.0f;
            float textureHeight = 1080.0f;
            float aspectX = textureWidth / (float)BackEnd::GetCurrentWindowWidth();
            float aspectY = textureHeight / (float)BackEnd::GetCurrentWindowHeight();
            int x = Input::GetMouseX() * aspectX;
            int y = textureHeight - (Input::GetMouseY() * aspectY);
            BackEnd::UpdateMousePicking(x, y);
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
}