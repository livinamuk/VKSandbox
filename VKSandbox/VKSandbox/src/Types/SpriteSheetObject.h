#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include <string>

struct SpriteSheetObject {
public:
    SpriteSheetObject(SpriteSheetObjectCreateInfo createInfo);
    void Update(float deltaTime);
    glm::mat4 GetModelMatrix();
    glm::mat4 GetBillboardModelMatrix(glm::vec3 cameraForward, glm::vec3 cameraRight, glm::vec3 cameraUp);
    uint32_t GetFrameIndex();
    uint32_t GetNextFrameIndex();
    bool IsBillboard();
    float GetMixFactor();
    const std::string& GetTextureName();
    glm::vec3 GetPosition();
    bool IsComplete();

private:
    uint32_t m_textureIndex = 0;
    Transform m_transform;
    uint32_t m_frameIndex = 0;
    uint32_t m_frameIndexNext = 1;
    float m_time = 0;
    float m_animationSpeed = 3.5f;
    bool m_loop = true;
    bool m_billboard = true;
    std::string m_textureName;
    uint32_t m_frameCount = 0;
    float m_mixFactor = 0.0f;
    bool m_animationComplete = false;
};