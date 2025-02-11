#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "HellEnums.h"
#include "Camera/Frustum.h"

struct SpaceCoords {
    float width;
    float height;
    float localMouseX;
    float localMouseY;
};

struct Viewport {
public:
    Viewport(const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, bool isOrthographic = true);
    void Update();
    void SetOrthographic(float orthoSize, float nearPlane, float farPlane);
    void SetPerspective(float fov, float nearPlane, float farPlane);
    void SetPosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    void Show();
    void Hide();
    void SetViewportMode(ShadingMode viewportMode);
    void SetOrthoSize(float value);
    const bool IsVisible() const;
    const bool IsOrthographic() const;
    const bool IsHovered() const;
    const float GetOrthoSize() const;
    const float GetPerspectiveFOV() const;
    glm::vec2 GetPosition() const;
    glm::vec2 GetSize() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetPerpsectiveMatrix() const;
    glm::mat4 GetOrthographicMatrix() const;
    ShadingMode GetViewportMode() const;
    SpaceCoords GetWindowSpaceCoords() const;
    SpaceCoords GetGBufferSpaceCoords() const;
    Frustum& GetFrustum() { return m_frustum; }

private:
    glm::vec2 m_position;           // Top-left corner in normalized screen space (0-1)
    glm::vec2 m_size;               // Width and height in normalized screen space (0-1)
    bool m_isOrthographic = false;  // True for orthographic, false for perspective
    float m_orthoSize;
    float m_nearPlane;
    float m_farPlane;
    float m_fov;
    float m_aspect;
    float m_leftPixel;
    float m_rightPixel;
    float m_topPixel;
    float m_bottomPixel;
    bool m_isVisible = true;
    bool m_hasHover = false;
    glm::mat4 m_perspectiveMatrix;
    glm::mat4 m_orthographicMatrix;
    glm::vec3 m_mouseRayDirPerspective;
    glm::vec3 m_mouseRayDirOrthographic;
    Frustum m_frustum;
    ShadingMode m_viewportMode;
    SpaceCoords m_windowSpaceCoords;
    SpaceCoords m_gBufferSpaceCoords;
    void UpdateProjectionMatrices();
};
