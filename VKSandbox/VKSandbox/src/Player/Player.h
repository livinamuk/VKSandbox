#pragma once
#include "HellTypes.h"
#include "Camera/Camera.h"

struct Player {
    void Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex);
    void Update(float deltaTime);
    void EnableControl();
    void DisableControl();
    const bool HasControl();
    const bool IsLocal() const;
    const bool IsOnline() const;
    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetInverseViewMatrix() const;
    const glm::vec3& GetCameraPosition() const;
    const glm::vec3& GetCameraRotation() const;
    const glm::vec3& GetCameraForward() const;
    const glm::vec3& GetCameraRight() const;
    const glm::vec3& GetCameraUp() const;
    const int32_t GetViewportIndex() const;
    Camera& GetCamera();

    void UpdateCamera();
    void UpdateMovement(float deltaTime);
    void UpdateUI();

private:
    Camera m_camera;
    glm::vec3 m_position;
    float m_mouseSensitivity = 0.002f;
    float m_walkSpeed = 5.0f;
    int32_t m_viewportIndex;
    bool m_controlEnabled = true;
};