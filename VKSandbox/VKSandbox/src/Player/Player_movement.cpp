#include "Player.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Editor/Editor.h"
#include "../Input/Input.h"

void Player::UpdateMovement(float deltaTime) {
    m_moving = false;
    m_crouching = PressingCrouch();
    m_groundedLastFrame = m_grounded;

    if (!ViewportIsVisible()) {
        //return;
    }

    if (!Editor::IsEditorOpen() && m_controlEnabled) {
        // Speed factor
        float speedFactor = 1.0f;
        if (Input::KeyDown(GLFW_KEY_LEFT_SHIFT)) {
            speedFactor = 5.0f;
        }

        // View height
        float heightSpeed = 3.0f;
        if (Input::KeyDown(HELL_KEY_EQUAL)) {
            m_position.y += deltaTime * heightSpeed * speedFactor;
        }
        if (Input::KeyDown(GLFW_KEY_MINUS)) {
            m_position.y -= deltaTime * heightSpeed * speedFactor;
        }

        // WSAD
        glm::vec3 displacement = glm::vec3(0);
        if (PressingWalkLeft()) {
            displacement -= m_camera.GetRight();
            m_moving = true;
        }
        if (PressingWalkRight()) {
            displacement += m_camera.GetRight();
            m_moving = true;
        }
        if (PressingWalkForward()) {
            displacement += m_camera.GetForwardXZ();
            m_moving = true;
        }
        if (PressingWalkBackward()) {
            displacement -= m_camera.GetForwardXZ();
            m_moving = true;
        }

        // Calculate movement speed
        float targetSpeed = m_crouching ? m_crouchingSpeed : m_walkingSpeed;
        float interpolationSpeed = 18.0f;
        if (!IsMoving()) {
            targetSpeed = 0.0f;
            interpolationSpeed = 22.0f;
        }
        m_currentSpeed = Util::FInterpTo(m_currentSpeed, targetSpeed, deltaTime, interpolationSpeed);

        displacement *= m_currentSpeed * deltaTime * speedFactor;


        // Jump
        if (PresingJump() && HasControl() && m_grounded) {
            m_yVelocity = 4.9f;  // better magic value for jump strength
            m_grounded = false;
        }
        //if (IsOverlappingLadder()) {
        //    m_grounded = true;
        //}

        // Gravity
        if (m_grounded) {
            m_yVelocity = -3.5f;
        }
        else {
            float gravity = 15.75f; // 9.8 feels like the moon
            m_yVelocity -= gravity * deltaTime;
        }
        displacement.y += m_yVelocity * deltaTime;
        MoveCharacterController(glm::vec3(displacement.x, displacement.y, displacement.z));

        // Check grounded state
        m_grounded = false;
        for (CharacterCollisionReport& report : Physics::GetCharacterCollisionReports()) {
            m_grounded = (report.characterController == m_characterController && report.hitNormal.y > 0.5f);
        }
        Physics::ClearCharacterControllerCollsionReports();
    }

}

bool Player::IsMoving() {
    return m_moving;
}

bool Player::IsCrouching() {
    return m_crouching;
}

bool Player::IsGrounded() {
    return m_grounded;
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
