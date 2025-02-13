#include "SpriteSheetObject.h"
#include "AssetManagement/AssetManager.h"
#include "Util/Util.h"

SpriteSheetObject::SpriteSheetObject(const SpriteSheetObjectCreateInfo& createInfo) {
    Init(createInfo);
}

void SpriteSheetObject::Init(const SpriteSheetObjectCreateInfo& createInfo) {
    m_position = createInfo.position;
    m_rotation = createInfo.rotation;
    m_scale = createInfo.scale;
    m_loop = createInfo.loop;
    m_billboard = createInfo.billboard;
    m_textureName = createInfo.textureName;
    m_animationSpeed = createInfo.animationSpeed;
    m_renderingEnabled = createInfo.renderingEnabled;
    m_spriteSheetTexture = AssetManager::GetSpriteSheetTextureByName(m_textureName);
}

void SpriteSheetObject::Update(float deltaTime) {
    // Calculate animation
    uint32_t frameCount = m_spriteSheetTexture->GetFrameCount();
    float frameDuration = 1.0f / m_animationSpeed;
    float totalAnimationTime = frameCount * frameDuration;
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
        m_frameIndexNext = m_loop ? (m_frameIndex + 1) % frameCount : std::min(m_frameIndex + 1, frameCount - 1);
    }
    m_animationComplete = !m_loop && m_time >= totalAnimationTime;
    m_timeAsPercentage = totalAnimationTime > 0.0f ? (m_time / totalAnimationTime) : 0.0f;

    // Construct render item
    if (m_renderingEnabled) {
        m_renderItem.frameIndex = m_frameIndex;
        m_renderItem.frameIndexNext = m_frameIndexNext;
        m_renderItem.columnCount = m_spriteSheetTexture->GetColumnCount();
        m_renderItem.rowCount = m_spriteSheetTexture->GetRowCount();
        m_renderItem.mixFactor = m_mixFactor;
        m_renderItem.textureIndex = m_spriteSheetTexture->GetTextureIndex();
        m_renderItem.isBillboard = (int)m_billboard;
        m_renderItem.position = glm::vec4(m_position, 1.0);
        m_renderItem.rotation = glm::vec4(m_rotation, 0.0);
        m_renderItem.scale = glm::vec4(m_scale, 0.0);
    }
}

void SpriteSheetObject::SetPosition(glm::vec3 position) {
    m_position = position;
}

void SpriteSheetObject::SetRotation(glm::vec3 rotation) {
    m_rotation = rotation;
}

void SpriteSheetObject::SetScale(glm::vec3 scale) {
    m_scale = scale;
}

void SpriteSheetObject::SetTime(float time) {
    m_time = time;
}

void SpriteSheetObject::SetSpeed(float speed) {
    m_animationSpeed= speed;
}

void SpriteSheetObject::EnableRendering() {
    m_renderingEnabled = true;
}

void SpriteSheetObject::DisableRendering() {
    m_renderingEnabled = false;
}