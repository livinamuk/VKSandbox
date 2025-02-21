#include "Util.h"
#include "HellDefines.h"
#include <numeric>
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

    glm::vec3 LerpVec3(glm::vec3 current, glm::vec3 target, float deltaTime, float interpSpeed) {
        glm::vec3 result;
        result.x = FInterpTo(current.x, target.x, deltaTime, interpSpeed);
        result.y = FInterpTo(current.y, target.y, deltaTime, interpSpeed);
        result.z = FInterpTo(current.z, target.z, deltaTime, interpSpeed);
        return result;
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

    float MapRange(float inValue, float minInRange, float maxInRange, float minOutRange, float maxOutRange) {
        float x = (inValue - minInRange) / (maxInRange - minInRange);
        return minOutRange + (maxOutRange - minOutRange) * x;
    }

    void InterpolateQuaternion(glm::quat& Out, const glm::quat& Start, const glm::quat& End, float pFactor) {
        // calc cosine theta
        float cosom = Start.x * End.x + Start.y * End.y + Start.z * End.z + Start.w * End.w;
        // adjust signs (if necessary)
        glm::quat end = End;
        if (cosom < static_cast<float>(0.0)) {
            cosom = -cosom;
            end.x = -end.x;   // Reverse all signs
            end.y = -end.y;
            end.z = -end.z;
            end.w = -end.w;
        }
        // Calculate coefficients
        float sclp, sclq;
        if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
        {
            // Standard case (slerp)
            float omega, sinom;
            omega = std::acos(cosom); // extract theta from dot product's cos theta
            sinom = std::sin(omega);
            sclp = std::sin((static_cast<float>(1.0) - pFactor) * omega) / sinom;
            sclq = std::sin(pFactor * omega) / sinom;
        }
        else {
            // Very close, do linear interp (because it's faster)
            sclp = static_cast<float>(1.0) - pFactor;
            sclq = pFactor;
        }
        Out.x = sclp * Start.x + sclq * end.x;
        Out.y = sclp * Start.y + sclq * end.y;
        Out.z = sclp * Start.z + sclq * end.z;
        Out.w = sclp * Start.w + sclq * end.w;
    }

    void NormalizeWeights(std::vector<float>& weights) {
        // Calculate the sum of all weights
        float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
        // Check if the sum is non-zero to avoid division by zero
        if (sum == 0.0f) {
            throw std::invalid_argument("Sum of weights cannot be zero.");
        }
        // Normalize each weight
        for (float& weight : weights) {
            weight /= sum;
        }
    }

    bool IsNan(float value) {
        return glm::isnan(value);
    }

    bool IsNan(glm::vec2 value) {
        return glm::isnan(value.x) || glm::isnan(value.y);
    }

    bool IsNan(glm::vec3 value) {
        return glm::isnan(value.x) || glm::isnan(value.y) || glm::isnan(value.z);
    }

    bool IsNan(glm::vec4 value) {
        return glm::isnan(value.x) || glm::isnan(value.y) || glm::isnan(value.z) || glm::isnan(value.w);
    }
}