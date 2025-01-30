#include "Viewport.h"
#include "../BackEnd/BackEnd.h"
#include "../Config/Config.h"

Viewport::Viewport(const glm::vec2& position, const glm::vec2& size, bool isOrthographic)
    : m_position(position),
    m_size(size),
    m_isOrthographic(isOrthographic),
    m_orthoSize(1.0f),
    m_nearPlane(NEAR_PLANE),
    m_farPlane(FAR_PLANE),
    m_fov(1.0f),
    m_perspectiveMatrix(glm::mat4(1.0f)),
    m_orthographicMatrix(glm::mat4(1.0f)),
    m_viewportMode(ViewportMode::SHADED) {
    UpdateProjectionMatrices();
}

void Viewport::UpdateHover() {

}

void Viewport::SetPerspective(float fov, float nearPlane, float farPlane) {
    m_isOrthographic = false;
    m_fov = fov;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    UpdateProjectionMatrices();
}

void Viewport::SetOrthographic(float orthoSize, float nearPlane, float farPlane) {
    m_isOrthographic = true;
    m_orthoSize = orthoSize;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    UpdateProjectionMatrices();
}

void Viewport::UpdateProjectionMatrices() {
    const Resolutions& resolutions = Config::GetResolutions();

    int renderTargetWidth = resolutions.gBuffer.x;
    int renderTargetHeight = resolutions.gBuffer.y;
    float viewportWidth = m_size.x * renderTargetWidth;
    float viewportHeight = m_size.y * renderTargetHeight;
    m_aspect = viewportWidth / viewportHeight;
    m_perspectiveMatrix = glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);

    float left = -m_orthoSize * m_aspect;
    float right = m_orthoSize * m_aspect;
    float bottom = -m_orthoSize;
    float top = m_orthoSize;
    m_orthographicMatrix = glm::orthoZO(left, right, bottom, top, m_nearPlane, m_farPlane);
}

glm::mat4 Viewport::GetProjectionMatrix() const {
    if (IsOrthographic()) {
        return m_orthographicMatrix;
    }
    else {
        return m_perspectiveMatrix;
    }
}

glm::mat4 Viewport::GetPerpsectiveMatrix() const {
    return m_perspectiveMatrix;
}

glm::mat4 Viewport::GetOrthographicMatrix() const {
    return m_orthographicMatrix;
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
    UpdateProjectionMatrices();
}

void Viewport::Show() {
    m_isVisible = true;
}

void Viewport::Hide() {
    m_isVisible = false;
}

void Viewport::SetViewportMode(ViewportMode viewportMode) {
    m_viewportMode = viewportMode;
}

void Viewport::NextViewportMode() {
    m_viewportMode = ViewportMode((int(m_viewportMode) + 1) % int(ViewportMode::VIEWPORT_MODE_COUNT));
}

const bool Viewport::IsVisible() const {
    return m_isVisible;
}

const bool Viewport::IsOrthographic() const {
    return m_isOrthographic;
}

ViewportMode Viewport::GetViewportMode() const {
    return m_viewportMode;
}

const bool Viewport::IsHovered() const {
    return m_hasHover;
}