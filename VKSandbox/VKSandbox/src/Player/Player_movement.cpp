#include "Player.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Editor/Editor.h"
#include "../Input/Input.h"

void Player::UpdateMovement(float deltaTime) {
    m_Moving = false;

    if (!Editor::IsOpen() && m_controlEnabled) {
        // Speed factor
        float speedFactor = 1.0f;
        if (Input::KeyDown(GLFW_KEY_LEFT_SHIFT)) {
            speedFactor = 0.425f;
        }

        // View height
        float heightSpeed = 3.0f;
        if (Input::KeyDown(HELL_KEY_SPACE)) {
            m_position.y += deltaTime * heightSpeed * speedFactor;
        }
        if (Input::KeyDown(GLFW_KEY_LEFT_CONTROL)) {
            m_position.y -= deltaTime * heightSpeed * speedFactor;
        }

        // WSAD
        glm::vec3 displacement = glm::vec3(0);
        if (Input::KeyDown(HELL_KEY_A)) {
            displacement -= m_camera.GetRight();
            m_Moving = true;
        }
        if (Input::KeyDown(HELL_KEY_D)) {
            displacement += m_camera.GetRight();
            m_Moving = true;
        }
        if (Input::KeyDown(HELL_KEY_W)) {
            displacement += m_camera.GetForwardXZ();
            m_Moving = true;
        }
        if (Input::KeyDown(HELL_KEY_S)) {
            displacement -= m_camera.GetForwardXZ();
            m_Moving = true;
        }

        displacement *= m_walkSpeed * deltaTime * speedFactor;
        m_position += displacement;
    }
}

bool Player::IsMoving() {
    return m_Moving;
}

bool Player::FeetEnteredUnderwater() {
    return false;
}

bool Player::FeetExitedUnderwater() {
    return false;
}

bool Player::CameraEnteredUnderwater() {
    return false;
}

bool Player::CameraExitedUnderwater() {
    return false;
}

bool Player::IsSwimming() {
    return false;
}

bool Player::IsWading() {
    return false;
}

bool Player::CameraIsUnderwater() {
    return false;
}

bool Player::FeetBelowWater() {
    return false;
}

bool Player::StartedSwimming() {
    return false;
}

bool Player::StoppedSwimming() {
    return false;
}

bool Player::StartingWading() {
    return false;
}

bool Player::StoppedWading() {
    return false;
}
