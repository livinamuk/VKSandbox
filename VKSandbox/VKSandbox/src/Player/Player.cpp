#include "Player.h"
#include "HellDefines.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Core/Game.h"
#include "../Editor/Editor.h"
#include "../Input/Input.h"
#include "../BackEnd/BackEnd.h"

void Player::Init(glm::vec3 position, glm::vec3 rotation, uint32_t viewportIndex) {
    m_position = position;
    m_camera.SetEulerRotation(rotation);
    m_viewportIndex = viewportIndex;
}

void Player::Update(float deltaTime) {

    if (!Editor::IsOpen()) {
        // Mouselook
        m_camera.AddPitch(-Input::GetMouseOffsetY() * m_mouseSensitivity);
        m_camera.AddYaw(-Input::GetMouseOffsetX() * m_mouseSensitivity);

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
        }
        if (Input::KeyDown(HELL_KEY_D)) {
            displacement += m_camera.GetRight();
        }
        if (Input::KeyDown(HELL_KEY_W)) {
            displacement += m_camera.GetForwardXZ();
        }
        if (Input::KeyDown(HELL_KEY_S)) {
            displacement -= m_camera.GetForwardXZ();
        }
        displacement *= m_walkSpeed * deltaTime * speedFactor;
        m_position += displacement;
        m_camera.SetPosition(m_position);
    }
}

const bool Player::IsLocal() const {
    return m_viewportIndex != -1;
}

const bool Player::IsOnline() const {
    return m_viewportIndex == -1;
}

const bool Player::IsViewportVisible() const {
    if (IsLocal()) {
        if (Game::GetSplitscreenMode() == SplitscreenMode::FULLSCREEN) {
            return (m_viewportIndex < 1);
        }
        if (Game::GetSplitscreenMode() == SplitscreenMode::TWO_PLAYER) {
            return (m_viewportIndex < 2);
        }
        if (Game::GetSplitscreenMode() == SplitscreenMode::FOUR_PLAYER) {
            return (m_viewportIndex < 4);
        }
    }
    else {
        return false;
    }
}

const glm::mat4& Player::GetProjectionMatrix() const {
    int width = BackEnd::GetCurrentWindowWidth();
    int height = BackEnd::GetCurrentWindowHeight();
    return glm::perspective(1.0f, float(width) / float(height), NEAR_PLANE, FAR_PLANE);
}

const glm::mat4& Player::GetViewMatrix() const {
    return m_camera.GetViewMatrix();
}

const glm::mat4& Player::GetInverseViewMatrix() const {
    return m_camera.GetInverseViewMatrix();
}

const glm::vec3& Player::GetCameraPosition() const {
    return m_camera.GetPosition();
}

const glm::vec3& Player::GetCameraRotation() const {
    return m_camera.GetEulerRotation();
}

const glm::vec3& Player::GetCameraForward() const {
    return m_camera.GetForward();
}

const glm::vec3& Player::GetCameraRight() const {
    return m_camera.GetRight();
}

const glm::vec3& Player::GetCameraUp() const {
    return m_camera.GetUp();
}

const uint32_t Player::GetViewportIndex() const {
    return m_viewportIndex;
}
