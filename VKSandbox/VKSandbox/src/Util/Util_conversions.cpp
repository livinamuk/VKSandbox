#include "Util.h"

namespace Util {

    std::string LightTypeToString(LightType type) {
        switch (type) {
        case LightType::LAMP_POST: return "LAMP_POST";
        default: return "UNKNOWN";
        }
    }

    LightType StringToLightType(const std::string& str) {
        if (str == "LAMP_POST") return LightType::LAMP_POST;
        return LightType::LAMP_POST;
    }

    std::string PickUpTypeToString(PickUpType type) {
        switch (type) {
        case PickUpType::SHOTGUN_AMMO_BUCKSHOT: return "SHOTGUN_AMMO_BUCKSHOT";
        case PickUpType::SHOTGUN_AMMO_SLUG: return "SHOTGUN_AMMO_SLUG";
        default: return "UNKNOWN";
        }
    }

    PickUpType StringToPickUpType(const std::string& str) {
        if (str == "SHOTGUN_AMMO_BUCKSHOT") return PickUpType::SHOTGUN_AMMO_BUCKSHOT;
        if (str == "SHOTGUN_AMMO_SLUG") return PickUpType::SHOTGUN_AMMO_SLUG;
        return PickUpType::SHOTGUN_AMMO_BUCKSHOT;
    }

    std::string BlendingModeToString(BlendingMode mode) {
        switch (mode) {
        case BlendingMode::NONE:                return "NONE";
        case BlendingMode::BLENDED:             return "BLENDED";
        case BlendingMode::ALPHA_DISCARDED:     return "ALPHA_DISCARDED";
        case BlendingMode::HAIR_UNDER_LAYER:    return "HAIR_UNDER_LAYER";
        case BlendingMode::HAIR_TOP_LAYER:      return "HAIR_TOP_LAYER";
        case BlendingMode::DO_NOT_RENDER:       return "DO_NOT_RENDER";
        default: return "UNKNOWN";
        }
    }
    
    BlendingMode StringToBlendingMode(const std::string& str) {
        if (str == "BLENDED") return BlendingMode::BLENDED;
        if (str == "ALPHA_DISCARDED") return BlendingMode::ALPHA_DISCARDED;
        if (str == "HAIR_UNDER_LAYER") return BlendingMode::HAIR_UNDER_LAYER;
        if (str == "HAIR_TOP_LAYER") return BlendingMode::HAIR_TOP_LAYER;
        if (str == "DO_NOT_RENDER") return BlendingMode::DO_NOT_RENDER;
        return BlendingMode::NONE;
    }
}
