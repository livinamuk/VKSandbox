#include "Player.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Editor/Editor.h"
#include "../Input/Input.h"

void Player::UpdateMovement(float deltaTime) {
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
    }
}