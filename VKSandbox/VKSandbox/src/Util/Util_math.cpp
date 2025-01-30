#include "Util.h"
#include <random>

namespace Util {
    glm::ivec2 WorldToScreenCoords(const glm::vec3& worldPos, const glm::mat4& viewProjection, int screenWidth, int screenHeight, bool flipY) {
        glm::vec4 clipCoords = viewProjection * glm::vec4(worldPos, 1.0f);
        glm::vec3 ndcCoords = glm::vec3(clipCoords) / clipCoords.w;
        glm::ivec2 screenCoords;
        screenCoords.x = (ndcCoords.x + 1.0f) * 0.5f * screenWidth;
        screenCoords.y = flipY
            ? screenHeight - (ndcCoords.y + 1.0f) * 0.5f * screenHeight
            : (1.0f - ndcCoords.y) * 0.5f * screenHeight;
        return screenCoords;
    }

    float FInterpTo(float current, float target, float deltaTime, float interpSpeed) {
        if (interpSpeed <= 0.f)
            return target;
        const float Dist = target - current;
        if (Dist * Dist < 9.99999993922529e-9f)
            return target;
        return current + Dist * glm::clamp(deltaTime * interpSpeed, 0.0f, 1.0f);
    }

    float RandomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / (RAND_MAX / (max - min));
    }

    int RandomInt(int min, int max) {
        static std::random_device dev;
        static std::mt19937 rng(dev());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
}