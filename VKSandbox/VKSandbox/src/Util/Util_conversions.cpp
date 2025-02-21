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
}