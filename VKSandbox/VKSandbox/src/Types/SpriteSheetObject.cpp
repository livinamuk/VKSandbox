#include "SpriteSheetObject.h"
#include "AssetManagement/AssetManager.h"

SpriteSheetObject::SpriteSheetObject(SpriteSheetObjectCreateInfo createInfo) {
    m_transform.position = createInfo.position;
    m_transform.rotation = createInfo.rotation;
    m_transform.scale = createInfo.scale;
    m_loop = createInfo.loop;
    m_billboard = createInfo.billboard;
    m_textureName = createInfo.textureName;
    m_animationSpeed = createInfo.animationSpeed;


   //m_textureName = AssetManager::GetTextureIndexByName(m_textureName);
   //
   //
   //m_frameCount = SpriteSheetTexture->m_frameCount;
  
}

void SpriteSheetObject::Update(float deltaTime) {
    int frameCount = m_frameCount;
    float frameDuration = 1.0f / m_animationSpeed;
    float totalAnimationTime = frameCount * frameDuration;
    m_loop = false;
    if (!m_loop) {
        m_time = std::min(m_time + deltaTime, totalAnimationTime);
    }
    else {
        m_time += deltaTime;
        if (m_time >= totalAnimationTime) {
            m_time = fmod(m_time, totalAnimationTime);
        }
    }
    float frameTime = m_time / frameDuration;
    m_mixFactor = fmod(frameTime, 1.0f);
    if (!m_loop && m_time >= totalAnimationTime) {
        m_frameIndex = frameCount - 1;
        m_frameIndexNext = m_frameIndex;
    }
    else {
        m_frameIndex = static_cast<int>(floor(frameTime)) % frameCount;
        m_frameIndexNext = m_loop ? (m_frameIndex + 1) % m_frameCount : std::min(m_frameIndex + 1, m_frameCount - 1);
    }
    m_animationComplete = !m_loop && m_time >= totalAnimationTime;
}

glm::mat4 SpriteSheetObject::GetModelMatrix() {
    return m_transform.to_mat4();
}

glm::mat4 SpriteSheetObject::GetBillboardModelMatrix(glm::vec3 cameraForward, glm::vec3 cameraRight, glm::vec3 cameraUp) {
    glm::mat4 billboardMatrix = glm::mat4(1.0f);
    billboardMatrix[0] = glm::vec4(cameraRight, 0.0f);
    billboardMatrix[1] = glm::vec4(cameraUp, 0.0f);
    billboardMatrix[2] = glm::vec4(cameraForward, 0.0f);
    billboardMatrix[3] = glm::vec4(m_transform.position, 1.0f);
    Transform scaleTransform;
    scaleTransform.scale = m_transform.scale;
    return billboardMatrix * scaleTransform.to_mat4();
}

uint32_t SpriteSheetObject::GetFrameIndex() {
    return m_frameIndex;
}

uint32_t SpriteSheetObject::GetNextFrameIndex() {
    return m_frameIndexNext;

}

bool SpriteSheetObject::IsBillboard() {
    return m_billboard;
}

float SpriteSheetObject::GetMixFactor() {
    return m_mixFactor;
}

const std::string& SpriteSheetObject::GetTextureName() {
    return m_textureName;
}

glm::vec3 SpriteSheetObject::GetPosition() {
    return m_transform.position;
}

bool SpriteSheetObject::IsComplete() {
    return !m_loop && m_animationComplete;
}