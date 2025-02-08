#include "Util.h"

namespace Util {
    bool StrCmp(const char* queryA, const char* queryB) {
        if (strcmp(queryA, queryB) == 0)
            return true;
        else
            return false;
    }

    std::string Lowercase(std::string& str) {
        std::string result;
        for (auto& c : str) {
            result += std::tolower(c);
        }
        return result;
    }

    std::string Uppercase(std::string& str) {
        std::string result;
        for (auto& c : str) {
            result += std::toupper(c);
        }
        return result;
    }

    std::string BoolToString(bool b) {
        return b ? "TRUE" : "FALSE";
    }

    std::string Vec3ToString(glm::vec3 v) {
        return std::format("({:.2f}, {:.2f}, {:.2f})", v.x, v.y, v.z);
    }

    glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b) {
        return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
    }

    glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b) {
        return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
    }

    std::string Mat4ToString(glm::mat4 m) {
        return std::format("{:.2f} {:.2f} {:.2f} {:.2f}\n{:.2f} {:.2f} {:.2f} {:.2f}\n{:.2f} {:.2f} {:.2f} {:.2f}\n{:.2f} {:.2f} {:.2f} {:.2f}",
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    std::string ViewportModeToString(const ShadingMode& viewportMode) {
        switch (viewportMode) {
        case ShadingMode::SHADED:            return "SHADED";
        case ShadingMode::WIREFRAME:         return "WIREFRAME";
        case ShadingMode::WIREFRAME_OVERLAY: return "WIREFRAME_OVERLAY";
        default:                              return "UNDEFINED";
        }
    }

    std::string CameraViewToString(const CameraView& cameraView) {
        switch (cameraView) {
        case CameraView::PERSPECTIVE:   return "PERSPECTIVE";
        case CameraView::ORTHO:         return "ORTHOGRAPHIC";
        case CameraView::FRONT:         return "FRONT";
        case CameraView::BACK:          return "BACK";
        case CameraView::TOP:           return "TOP";
        case CameraView::BOTTOM:        return "BOTTOM";
        case CameraView::LEFT:          return "LEFT";
        case CameraView::RIGHT:         return "RIGHT";
        default:                        return "UNDEFINED"; 
        }
    }

    std::string ViewportResizeStateToString(const ViewportResizeState& viewportResizeState) {
        switch (viewportResizeState) {
        case ViewportResizeState::IDLE:                             return "IDLE";
        case ViewportResizeState::RESIZING_HORIZONTAL:              return "RESIZING_HORIZONTAL";
        case ViewportResizeState::RESIZING_VERTICAL:                return "RESIZING_VERTICAL";
        case ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL:     return "RESIZING_HORIZONTAL_VERTICAL";
        default:                                                    return "UNDEFINED";
        }
    }

    const char* CopyConstChar(const char* text) {
        char* b = new char[strlen(text) + 1] {};
        std::copy(text, text + strlen(text), b);
        return b;
    }

    std::string WeaponActionToString(const WeaponAction& weaponAction) {
        switch (weaponAction) {
        case WeaponAction::IDLE:                          return "IDLE";
        case WeaponAction::FIRE:                          return "FIRE";
        case WeaponAction::RELOAD:                        return "RELOAD";
        case WeaponAction::RELOAD_FROM_EMPTY:             return "RELOAD_FROM_EMPTY";
        case WeaponAction::DRAW_BEGIN:                    return "DRAW_BEGIN";
        case WeaponAction::DRAWING:                       return "DRAWING";
        case WeaponAction::SPAWNING:                      return "SPAWNING";
        case WeaponAction::RELOAD_SHOTGUN_BEGIN:          return "RELOAD_SHOTGUN_BEGIN";
        case WeaponAction::RELOAD_SHOTGUN_SINGLE_SHELL:   return "RELOAD_SHOTGUN_SINGLE_SHELL";
        case WeaponAction::RELOAD_SHOTGUN_DOUBLE_SHELL:   return "RELOAD_SHOTGUN_DOUBLE_SHELL";
        case WeaponAction::RELOAD_SHOTGUN_END:            return "RELOAD_SHOTGUN_END";
        case WeaponAction::RELOAD_REVOLVER_BEGIN:         return "RELOAD_REVOLVER_BEGIN";
        case WeaponAction::RELOAD_REVOLVER_LOOP:          return "RELOAD_REVOLVER_LOOP";
        case WeaponAction::RELOAD_REVOLVER_END:           return "RELOAD_REVOLVER_END";
        case WeaponAction::ADS_IN:                        return "ADS_IN";
        case WeaponAction::ADS_OUT:                       return "ADS_OUT";
        case WeaponAction::ADS_IDLE:                      return "ADS_IDLE";
        case WeaponAction::ADS_FIRE:                      return "ADS_FIRE";
        case WeaponAction::MELEE:                         return "MELEE";
        default:                                          return "UNDEFINED";
        }
    }
}