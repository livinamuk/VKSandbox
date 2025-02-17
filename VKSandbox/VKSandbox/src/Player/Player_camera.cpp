#include "Player.h"
#include "Core/Debug.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Input/InputMulti.h"
#include <glm/gtc/noise.hpp> 

#include "Renderer/Renderer.h"

void Player::UpdateHeadBob(float deltaTime) {
    if (IsMoving()) {
        m_headBobTime += deltaTime;
    }

    float walkSpeed = 3.2f;
    if (IsCrouching()) {
        walkSpeed *= 0.75f;
    }
    float bobIntensity = 0.05f;
    float noiseIntensity = 0.02f;
    float frequency = 4.5f * walkSpeed;
    m_bobOffsetY = glm::sin(m_headBobTime * frequency) * bobIntensity;
    m_bobOffsetX = glm::sin(m_headBobTime * frequency * 0.5f) * (bobIntensity * 0.5f);
    float noiseOffsetY = glm::perlin(glm::vec2(m_headBobTime * 0.1f, 0.0f)) * noiseIntensity;
    float noiseOffsetX = glm::perlin(glm::vec2(0.0f, m_headBobTime * 0.1f)) * noiseIntensity;
    m_headBob = glm::vec3(m_bobOffsetX + noiseOffsetX, m_bobOffsetY + noiseOffsetY, 0.0f);


    //if (rayResult.hitFound && rayResult.objectType == ObjectType::HEIGHT_MAP) {
    //    m_isOutside = true;
    //}
    //if (rayResult.hitFound && rayResult.objectType == ObjectType::CSG_OBJECT_ADDITIVE_CUBE ||
    //    rayResult.hitFound && rayResult.objectType == ObjectType::CSG_OBJECT_ADDITIVE_FLOOR_PLANE) {
    //    m_isOutside = false;
    //}

    // Flashlight
    m_flashlightPosition = GetCameraPosition();
    m_flashlightPosition += GetCameraRight() * glm::vec3(0.2f);
    m_flashlightPosition += GetCameraUp() * glm::vec3(m_bobOffsetY * 2);
    //m_flashlightDirection = GetCameraForward();
    glm::vec3 flashlightTargetPosition = m_flashlightPosition + m_flashlightDirection;
    glm::mat4 flashlightViewMatrix = glm::lookAt(m_flashlightPosition, flashlightTargetPosition, GetCameraUp());


    float lightRadius = 10.0f;
    float outerAngle = cos(glm::radians(25.0));
    glm::mat4 spotlightProjection = glm::perspective(outerAngle, 1.0f, 0.1f, lightRadius);

    m_flashlightProjectionView = spotlightProjection * flashlightViewMatrix;

    if (!ViewportIsVisible()) {
        return;
    }

    glm::vec3 rayOrigin = GetCameraPosition();
    glm::vec3 rayDir = GetCameraForward();
    PxU32 raycastFlags = RaycastGroup::RAYCAST_ENABLED;
    PhysXRayResult rayResult = Physics::CastPhysXRay(rayOrigin, rayDir, 50, raycastFlags);

    if (rayResult.hitFound) {
        glm::vec3 hitPosition = rayResult.hitPosition;
        //Renderer::DrawPoint(hitPosition, WHITE);
        m_flashlightDirection = glm::normalize(hitPosition - m_flashlightPosition);
    }
}

void Player::UpdateBreatheBob(float deltaTime) {
    if (Util::IsNan(GetCameraUp()) || Util::IsNan(GetCameraRight())) return;

    m_breatheBobTime += deltaTime;

    float breathSpeed = 0.5f;
    float horizontalBreathIntensity = 0.00025f;
    float verticalBreathIntensity = 0.002f;
    float noiseIntensity = 0.0005f;

    float breathOffsetX = glm::sin(m_breatheBobTime * breathSpeed * glm::two_pi<float>()) * horizontalBreathIntensity;
    float breathOffsetY = glm::sin(m_breatheBobTime * breathSpeed * glm::two_pi<float>() * 0.5f) * verticalBreathIntensity;

    float noiseOffsetX = glm::perlin(glm::vec2(m_breatheBobTime * 0.05f, 0.0f)) * noiseIntensity;
    float noiseOffsetY = glm::perlin(glm::vec2(0.0f, m_breatheBobTime * 0.05f)) * noiseIntensity;

    m_breatheBob = GetCameraUp() * (breathOffsetY + noiseOffsetY);
    m_breatheBob += GetCameraRight() * glm::vec3(breathOffsetX + noiseOffsetX);
}

void Player::UpdateCamera(float deltaTime) {
    // Mouselook
    if (!Editor::IsOpen() && m_controlEnabled) {
        float xOffset = (float)InputMulti::GetMouseXOffset(m_mouseIndex);
        float yOffset = (float)InputMulti::GetMouseYOffset(m_mouseIndex);
        m_camera.AddPitch(-yOffset * m_mouseSensitivity);
        m_camera.AddYaw(xOffset * m_mouseSensitivity);        
    }

    // Height
    float crouchDownSpeed = 17.5f;
    float viewHeightTarget = m_crouching ? m_viewHeightCrouching : m_viewHeightStanding;
    m_currentViewHeight = Util::FInterpTo(m_currentViewHeight, viewHeightTarget, deltaTime, crouchDownSpeed);

    // Position
    m_camera.SetPosition(m_position + glm::vec3(0, m_currentViewHeight, 0) + m_headBob + m_breatheBob);

    // Get view weapon camera matrix
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    SkinnedModel* model = viewWeapon->m_skinnedModel;

    glm::mat4 cameraMatrix = viewWeapon->GetAnimatedTransformByBoneName("camera");
    glm::mat4 dmMaster = viewWeapon->GetAnimatedTransformByBoneName("Dm-Master");
    glm::mat4 cameraBindMatrix;

    for (int i = 0; i < model->m_nodes.size(); i++) {
        if (Util::StrCmp(model->m_nodes[i].m_name, "camera")) {
            cameraBindMatrix = model->m_nodes[i].m_inverseBindTransform;
        }
    }
    m_viewWeaponCameraMatrix = inverse(cameraBindMatrix) * cameraMatrix;
}
