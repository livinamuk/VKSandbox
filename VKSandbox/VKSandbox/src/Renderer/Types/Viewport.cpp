#include "Viewport.h"
#include "../BackEnd/BackEnd.h"

Viewport::Viewport(const glm::vec2& position, const glm::vec2& size, bool isOrthographic)
    : m_position(position),
    m_size(size),
    m_isOrthographic(isOrthographic),
    m_orthoSize(10.0f),
    m_nearPlane(0.1f),
    m_farPlane(100.0f),
    m_fov(45.0f),
    m_projectionMatrix(1.0f) {
    UpdateProjectionMatrix();
}

void Viewport::SetOrthographic(float orthoSize, float nearPlane, float farPlane) {
    m_isOrthographic = true;
    m_orthoSize = orthoSize;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    UpdateProjectionMatrix();
}

void Viewport::SetPerspective(float fov, float nearPlane, float farPlane) {
    m_isOrthographic = false;
    m_fov = fov;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    UpdateProjectionMatrix();
}

void Viewport::UpdateProjectionMatrix() {
    int screenWidth = BackEnd::GetCurrentWindowWidth();
    int screenHeight = BackEnd::GetCurrentWindowHeight();
    float viewportWidth = m_size.x * screenWidth;
    float viewportHeight = m_size.y * screenHeight;
    m_aspect = viewportWidth / viewportHeight;
    if (m_isOrthographic) {
        m_projectionMatrix = glm::ortho(-m_orthoSize * m_aspect, m_orthoSize * m_aspect,
            -m_orthoSize, m_orthoSize,
            m_nearPlane, m_farPlane);
    }
    else {
        m_projectionMatrix = glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
    }
}

glm::mat4 Viewport::GetProjectionMatrix() const {
    return m_projectionMatrix;
}

glm::vec2 Viewport::GetPosition() const {
    return m_position;
}

glm::vec2 Viewport::GetSize() const {
    return m_size;
}

void Viewport::SetPosition(const glm::vec2& position) {
    m_position = position;
}

void Viewport::SetSize(const glm::vec2& size) {
    m_size = size;
    UpdateProjectionMatrix();
}
