#include "Util.h"
#include "HellDefines.h"

namespace Util {

    glm::mat4 Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ) {
        return glm::scale(glm::mat4(1.0), glm::vec3(ScaleX, ScaleY, ScaleZ));
    }

    glm::mat4 Mat4InitTranslationTransform(float x, float y, float z) {
        return  glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    }

    glm::mat4 Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ) {
        glm::mat4 rx = glm::mat4(1);
        glm::mat4 ry = glm::mat4(1);
        glm::mat4 rz = glm::mat4(1);

        const float x = ToRadian(RotateX);
        const float y = ToRadian(RotateY);
        const float z = ToRadian(RotateZ);

        rx[0][0] = 1.0f; rx[0][1] = 0.0f; rx[0][2] = 0.0f; rx[0][3] = 0.0f;
        rx[1][0] = 0.0f; rx[1][1] = cosf(x); rx[1][2] = -sinf(x); rx[1][3] = 0.0f;
        rx[2][0] = 0.0f; rx[2][1] = sinf(x); rx[2][2] = cosf(x); rx[2][3] = 0.0f;
        rx[3][0] = 0.0f; rx[3][1] = 0.0f; rx[3][2] = 0.0f; rx[3][3] = 1.0f;

        ry[0][0] = cosf(y); ry[0][1] = 0.0f; ry[0][2] = -sinf(y); ry[0][3] = 0.0f;
        ry[1][0] = 0.0f; ry[1][1] = 1.0f; ry[1][2] = 0.0f; ry[1][3] = 0.0f;
        ry[2][0] = sinf(y); ry[2][1] = 0.0f; ry[2][2] = cosf(y); ry[2][3] = 0.0f;
        ry[3][0] = 0.0f; ry[3][1] = 0.0f; ry[3][2] = 0.0f; ry[3][3] = 1.0f;

        rz[0][0] = cosf(z); rz[0][1] = -sinf(z); rz[0][2] = 0.0f; rz[0][3] = 0.0f;
        rz[1][0] = sinf(z); rz[1][1] = cosf(z); rz[1][2] = 0.0f; rz[1][3] = 0.0f;
        rz[2][0] = 0.0f; rz[2][1] = 0.0f; rz[2][2] = 1.0f; rz[2][3] = 0.0f;
        rz[3][0] = 0.0f; rz[3][1] = 0.0f; rz[3][2] = 0.0f; rz[3][3] = 1.0f;

        return rz * ry * rx;
    }

    AnimatedTransform BlendTransforms(const AnimatedTransform& transformA, const AnimatedTransform& transformB, float blendFactor) {
        AnimatedTransform result;
        result.translation = glm::mix(transformA.translation, transformB.translation, blendFactor);
        result.rotation = glm::slerp(transformA.rotation, transformB.rotation, blendFactor);
        result.scale = glm::mix(transformA.scale, transformB.scale, blendFactor);
        return result;
    }

    AnimatedTransform BlendMultipleTransforms(const std::vector<AnimatedTransform>& transforms, const std::vector<float>& weights) {
        if (transforms.size() != weights.size() || transforms.empty()) {
            throw std::invalid_argument("Number of transforms and weights must be the same and non-zero.");
        }
        AnimatedTransform result;
        result.translation = glm::vec3(0.0f);
        result.rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
        result.scale = glm::vec3(0.0f);
        float totalWeight = 0.0f;
        for (size_t i = 0; i < transforms.size(); ++i) {
            result.translation += transforms[i].translation * weights[i];
            result.rotation = glm::slerp(result.rotation, transforms[i].rotation, weights[i]);
            result.scale += transforms[i].scale * weights[i];
            totalWeight += weights[i];
        }
        if (totalWeight > 0.0f) {
            result.translation /= totalWeight;
            result.scale /= totalWeight;
        }
        // Normalize rotation to maintain a valid quaternion (optional, depending on glm::slerp implementation)
        result.rotation = glm::normalize(result.rotation);
        return result;
    }

    float SmoothStep(float t) {
        t = glm::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    float SmoothStepReverse(float t) {
        t = glm::clamp(t, 0.0f, 1.0f);
        return 1.0f - (t * t * (3.0f - 2.0f * t));
    }

    float SteepSlowDownCurve(float t) {
        t = glm::clamp(t, 0.0f, 1.0f);
        return t * t * t;
    }

    float EaseOut(float t) {
        t = glm::clamp(t, 0.0f, 1.0f);
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }

    glm::mat4 aiMatrix3x3ToGlm(const aiMatrix3x3& from) {
        glm::mat4 to;
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = 0.0;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = 0.0;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = 0.0;
        to[0][3] = 0.0; to[1][3] = 0.0; to[2][3] = 0.0; to[3][3] = 1.0;
        return to;
    }
}