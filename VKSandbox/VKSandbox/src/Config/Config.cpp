#include "Config.h"

namespace Config {
    Resolutions g_resolitions;

    void Init() {
        g_resolitions.gBuffer = { 1920, 1080 };
        g_resolitions.finalImage = { 1920 / 2, 1080 / 2 };
        g_resolitions.ui = { 1920, 1080 };
        g_resolitions.hair = { 1920 / 2, 1080 / 2 };
    }

    const Resolutions& GetResolutions() {
        return g_resolitions;
    }

}