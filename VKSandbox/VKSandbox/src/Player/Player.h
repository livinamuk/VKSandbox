#pragma once
#include "HellTypes.h"
#include "../Types/Camera.h"

struct Player {
    void Init(glm::vec3 position, glm::vec3 rotation, uint32_t viewportIndex);
    void Update(float deltaTime);
    const bool IsLocal() const;
    const bool IsOnline() const;
    const bool IsViewportVisible() const;
    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetInverseViewMatrix() const;
    const glm::vec3& GetCameraPosition() const;
    const glm::vec3& GetCameraRotation() const;
    const glm::vec3& GetCameraForward() const;
    const glm::vec3& GetCameraRight() const;
    const glm::vec3& GetCameraUp() const;
    const uint32_t GetViewportIndex() const;

private:
    Camera m_camera;
    glm::vec3 m_position;
    float m_mouseSensitivity = 0.002f;
    float m_walkSpeed = 5.0f;
    uint32_t m_viewportIndex;
};