#pragma once
#include "../Renderer/Types/DetachedMesh.hpp"
#include <vector>

enum class GizmoMode {
    TRANSLATE,
    ROTATE,
    SCALE
};

enum class GizmoFlag {
    NONE,
    TRANSLATE_X,
    TRANSLATE_Y,
    TRANSLATE_Z,
    ROTATE_X,
    ROTATE_Y,
    ROTATE_Z,
    SCALE_X,
    SCALE_Y,
    SCALE_Z,
    SCALE,
};

enum class GizmoAction {
    IDLE,
    DRAGGING
};

struct GizmoRenderItem {
    glm::mat4 modelMatrix;
    glm::vec4 color;
    GizmoFlag flag;
    int meshIndex;
};

namespace Gizmo {
    void Init();
    void Update();
    void SetPosition(glm::vec3 position);
    void SetEuler(glm::vec3 euler);
    void UpdateRenderItems();

    std::vector<GizmoRenderItem>& GetRenderItemsByViewportIndex(int index);
    DetachedMesh* GetDetachedMeshByIndex(int index);
    const std::string GizmoFlagToString(const GizmoFlag& flag);
    const glm::vec3 GetPosition();
    const glm::vec3 GetEulerRotation();
    const bool HasHover();
    float GetGizmoScalingFactorByViewportIndex(int viewportIndex);
    const GizmoAction GetAction();
    const GizmoMode GetMode();
}