#include "Config.h"

namespace Config {

    glm::ivec2 g_uiResolution = { 1920, 1080 };

    glm::ivec2 GetUIResolution() {
        return g_uiResolution;
    }
}