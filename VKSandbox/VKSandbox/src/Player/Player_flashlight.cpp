#include "Player.h"
#include "Core/Audio.h"
#include "Util.h"

// GET THIS OUT OF HERE
// GET THIS OUT OF HERE
// GET THIS OUT OF HERE
#include "API/OpenGL/Renderer/GL_renderer.h"
// GET THIS OUT OF HERE
// GET THIS OUT OF HERE
// GET THIS OUT OF HERE


void Player::UpdateFlashlight(float deltaTime) {
    // Toggle on/off
    if (PressedFlashlight()) {
        Audio::PlayAudio("Flashlight.wav", 1.5f);
        m_flashlightOn = !m_flashlightOn;
    }
    // Modifier
    if (!m_flashlightOn) {
        m_flashLightModifier = 0.0f;
    }
    else {
        m_flashLightModifier = Util::FInterpTo(m_flashLightModifier, 1.0f, deltaTime, 10.5f);
    }

    // Position
    m_flashlightPosition = GetCameraPosition();
    m_flashlightPosition += GetCameraRight() * glm::vec3(0.3f);
    m_flashlightPosition -= GetCameraUp() * glm::vec3(m_bobOffsetY * 2);
    
    // Projection view matrix
    float lightRadius = 25.0f;
    float outerAngle = glm::radians(25.0);
    glm::vec3 flashlightTargetPosition = m_flashlightPosition + m_flashlightDirection;
    glm::mat4 flashlightViewMatrix = glm::lookAt(m_flashlightPosition, flashlightTargetPosition, GetCameraUp());
    glm::mat4 spotlightProjection = glm::perspectiveZO(outerAngle * 2, 1.0f, 0.05f, lightRadius);
    m_flashlightProjectionView = spotlightProjection * flashlightViewMatrix;

    // Direction
    if (!ViewportIsVisible()) {
        return;
    }
    glm::vec3 rayOrigin = GetCameraPosition();
    glm::vec3 rayDir = GetCameraForward();

    // Prevent NAN direction, which is the case on first spawn
    if (Util::IsNan(m_flashlightDirection)) {
        m_flashlightDirection = GetCameraForward();
    }

    if (OpenGLRenderer::IsPlayerRayWorldPositionReadBackReady(m_viewportIndex)) {
        glm::vec3 target = GetCameraPosition() + GetCameraForward() * glm::vec3(4);

        // Unless crosshair hit position is less than 4 meters, then use ray hit pos
        glm::vec3 crosshairHitPosition = OpenGLRenderer::GetPlayerRayWorldPostion(m_viewportIndex);
        if (glm::distance(crosshairHitPosition, GetCameraPosition()) < 4) {
            target = crosshairHitPosition;
        }
        glm::vec3 flashlightDirectionTarget = glm::normalize(target - m_flashlightPosition);

        float interSpeed = 30;
        m_flashlightDirection = Util::LerpVec3(m_flashlightDirection, flashlightDirectionTarget, deltaTime, interSpeed);
    }
    // If worldspace position pixel readback is not ready, 
    // then fall back to the camera forward vector
    else {
        m_flashlightDirection = GetCameraForward();
    }
}