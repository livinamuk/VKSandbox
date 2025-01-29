#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Viewport {
public:
    Viewport(const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, bool isOrthographic = true);
    void SetOrthographic(float orthoSize, float nearPlane, float farPlane);
    void SetPerspective(float fov, float nearPlane, float farPlane);
    void SetPosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    glm::mat4 GetProjectionMatrix() const;
    glm::vec2 GetPosition() const;
    glm::vec2 GetSize() const;

private:
    glm::vec2 m_position;           // Top-left corner in normalized screen space (0-1)
    glm::vec2 m_size;               // Width and height in normalized screen space (0-1)
    bool m_isOrthographic = false;  // True for orthographic, false for perspective
    float m_orthoSize;
    float m_nearPlane;
    float m_farPlane;
    float m_fov;
    float m_aspect;
    glm::mat4 m_projectionMatrix;
    void UpdateProjectionMatrix();
};
