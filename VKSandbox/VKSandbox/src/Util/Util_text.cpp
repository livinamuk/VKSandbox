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

    std::string Vec2ToString(glm::vec2 v) {
        return std::format("({:.2f}, {:.2f})", v.x, v.y);
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

    std::string EditorStateToString(const EditorState& editorState) {
        switch (editorState) {
            case EditorState::IDLE:                             return "IDLE";
            case EditorState::RESIZING_HORIZONTAL:              return "RESIZING_HORIZONTAL";
            case EditorState::RESIZING_VERTICAL:                return "RESIZING_VERTICAL";
            case EditorState::RESIZING_HORIZONTAL_VERTICAL:     return "RESIZING_HORIZONTAL_VERTICAL";
            case EditorState::GIZMO_TRANSLATING:                return "GIZMO_TRANSLATING";
            case EditorState::GIZMO_SCALING:                    return "GIZMO_SCALING";
            case EditorState::GIZMO_ROTATING:                   return "GIZMO_ROTATING";
            case EditorState::DRAGGING_SELECT_RECT:             return "DRAGGING_SELECT_RECT";
        default:                                            return "UNDEFINED";
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
            case WeaponAction::DRAWING_FIRST:                 return "DRAWING_FIRST";
            case WeaponAction::DRAWING_WITH_SHOTGUN_PUMP:     return "DRAWING_WITH_SHOTGUN_PUMP";
            case WeaponAction::SPAWNING:                      return "SPAWNING";
            case WeaponAction::SHOTGUN_RELOAD_BEGIN:          return "SHOTGUN_RELOAD_BEGIN";
            case WeaponAction::SHOTGUN_RELOAD_SINGLE_SHELL:   return "SHOTGUN_RELOAD_SINGLE_SHELL";
            case WeaponAction::SHOTGUN_RELOAD_DOUBLE_SHELL:   return "SHOTGUN_RELOAD_DOUBLE_SHELL";
            case WeaponAction::SHOTGUN_RELOAD_END:            return "SHOTGUN_RELOAD_END";
            case WeaponAction::SHOTGUN_RELOAD_END_WITH_PUMP:  return "SHOTGUN_RELOAD_END_WITH_PUMP";
            case WeaponAction::ADS_IN:                        return "ADS_IN";
            case WeaponAction::ADS_OUT:                       return "ADS_OUT";
            case WeaponAction::ADS_IDLE:                      return "ADS_IDLE";
            case WeaponAction::ADS_FIRE:                      return "ADS_FIRE";
            case WeaponAction::MELEE:                         return "MELEE";
            case WeaponAction::TOGGLING_AUTO:                 return "TOGGLING_AUTO";
            case WeaponAction::DRY_FIRE:                      return "DRY_FIRE";
            default:                                          return "UNDEFINED";
        }
    }

    std::string ImageDataTypeToString(const ImageDataType& imageDataType) {
        switch (imageDataType) {
            case ImageDataType::UNCOMPRESSED: return "UNCOMPRESSED";
            case ImageDataType::COMPRESSED:   return "COMPRESSED";
            case ImageDataType::EXR:          return "EXR";
            default:                          return "UNDEFINED";
        }
    }
    std::string EditorModeToString(const EditorMode& editorMode) {
        switch (editorMode) {
            case EditorMode::SECTOR_EDITOR:     return "SECTOR_EDITOR";
            case EditorMode::HEIGHTMAP_EDITOR:  return "HEIGHTMAP_EDITOR";
            case EditorMode::WEAPON_EDITOR:     return "WEAPON_EDITOR";
            default:                            return "UNDEFINED";
        }
    }
}