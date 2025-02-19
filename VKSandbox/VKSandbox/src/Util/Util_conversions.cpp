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
}