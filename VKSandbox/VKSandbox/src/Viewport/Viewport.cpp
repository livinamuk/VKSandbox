#include "Viewport.h"
#include "HellDefines.h"
#include "../BackEnd/BackEnd.h"
#include "../Config/Config.h"
#include "../Input/Input.h"
#include "../Util/Util.h"

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
    m_viewportMode(ShadingMode::SHADED) {
    UpdateProjectionMatrices();
}

void Viewport::UpdateHover() {
    // Pixel bounds
    m_leftPixel = m_position.x * BackEnd::GetCurrentWindowWidth();
    m_rightPixel = m_leftPixel + m_windowSpaceCoords.width;
    m_topPixel = BackEnd::GetCurrentWindowHeight() - (m_position.y * BackEnd::GetCurrentWindowHeight());
    m_bottomPixel = m_topPixel - m_windowSpaceCoords.height;

    // Window space co-ordinates
    m_windowSpaceCoords.width = m_size.x * BackEnd::GetCurrentWindowWidth();
    m_windowSpaceCoords.height = m_size.y * BackEnd::GetCurrentWindowHeight();
    m_windowSpaceCoords.localMouseX = Input::GetMouseX();
    m_windowSpaceCoords.localMouseY = Input::GetMouseY();

    // GBuffer space co-ordinates
    const Resolutions& resolutions = Config::GetResolutions();
    m_gBufferSpaceCoords.width = resolutions.gBuffer.x * m_size.x;
    m_gBufferSpaceCoords.height = resolutions.gBuffer.y * m_size.y;
    m_gBufferSpaceCoords.localMouseX = Util::MapRange(Input::GetMouseX(), m_leftPixel, m_rightPixel, 0, m_gBufferSpaceCoords.width);
    m_gBufferSpaceCoords.localMouseY = Util::MapRange(Input::GetMouseY(), m_bottomPixel, m_topPixel, 0, m_gBufferSpaceCoords.height);

    // Viewport mouse hover state
    m_hasHover = (
        Input::GetMouseX() > m_leftPixel &&
        Input::GetMouseX() < m_rightPixel &&
        Input::GetMouseY() < m_topPixel &&
        Input::GetMouseY() > m_bottomPixel
    );
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

SpaceCoords Viewport::GetWindowSpaceCoords() const {
    return m_windowSpaceCoords;
}

SpaceCoords Viewport::GetGBufferSpaceCoords() const {
    return m_gBufferSpaceCoords;
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

void Viewport::SetViewportMode(ShadingMode viewportMode) {
    m_viewportMode = viewportMode;
}

void Viewport::SetOrthoSize(float value) {
    m_orthoSize = value;
    UpdateProjectionMatrices();
}

const float Viewport::GetOrthoSize() const {
    return m_orthoSize;
}

const float Viewport::GetPerspectiveFOV() const {
    return m_fov;
}

const bool Viewport::IsVisible() const {
    return m_isVisible;
}

const bool Viewport::IsOrthographic() const {
    return m_isOrthographic;
}

ShadingMode Viewport::GetViewportMode() const {
    return m_viewportMode;
}

const bool Viewport::IsHovered() const {
    return m_hasHover;
}