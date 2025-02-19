#include "Player.h"
#include "Core/Audio.h"
#include "Util.h"

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
    PxU32 raycastFlags = RaycastGroup::RAYCAST_ENABLED;
    PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDir, 250, raycastFlags);
    if (rayResult.hitFound) {
        glm::vec3 hitPosition = rayResult.hitPosition;
        m_flashlightDirection = glm::normalize(hitPosition - m_flashlightPosition);
        //Renderer::DrawPoint(hitPosition, WHITE);
    }
    m_flashlightDirection = GetCameraForward();
}