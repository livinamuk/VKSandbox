#include "Player.h"
#include "../Editor/Editor.h"
#include "../Input/Input.h"

void Player::UpdateCamera() {
    // Mouselook
    if (!Editor::IsOpen() && m_controlEnabled) {
        m_camera.AddPitch(-Input::GetMouseOffsetY() * m_mouseSensitivity);
        m_camera.AddYaw(-Input::GetMouseOffsetX() * m_mouseSensitivity);        
    }
    // Position
    m_camera.SetPosition(m_position);
}