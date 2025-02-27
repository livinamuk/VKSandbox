#include "AnimationState.h"
#include "Animation.h"
#include "AssetManagement/AssetManager.h"
#include "Util.h"

void AnimationState::PlayAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams) {
    int tempIndex = AssetManager::GetAnimationIndexByName(animationName);
    if (tempIndex != -1) {
        m_index = tempIndex;
        m_currentTime = 0;
        m_loop = false;
        m_paused = false;
        m_isComplete = false;
        m_easeOut = false;
        m_playbackParams = playbackParams;
        m_playbackParams.animationName = animationName;
        m_blendFactor = m_playbackParams.blendWeight;
        m_easeOutTimer = 0;
        return;
    }
    else {
        std::cout << "AnimationState::PlayAnimation() failed because " << animationName << " does not exist\n";
    }
}

void AnimationState::PlayAndLoopAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams) {
    int tempIndex = AssetManager::GetAnimationIndexByName(animationName);
    if (tempIndex == -1) {
        std::cout << "AnimationState::PlayAndLoopAnimation() failed because " << animationName << " does not exist\n";
        return;
    }
    else if (tempIndex == m_index) {
        m_loop = true;
        m_forceStop = false;
        m_isComplete = false;
        m_paused = false;
        m_easeOut = false;
        m_blendFactor = m_playbackParams.blendWeight;
        m_easeOutTimer = 0;
        return; // Optimize this to not be doing string searches every frame
    }
    else {
        m_index = tempIndex;
        m_currentTime = 0;
        m_paused = false;
        m_loop = true;
        m_isComplete = false;
        m_easeOut = false;
        m_playbackParams = playbackParams;
        m_playbackParams.animationName = animationName;
        m_blendFactor = m_playbackParams.blendWeight;
        m_easeOutTimer = 0;
        //std::cout << "playing and looping " << animationName << "\n";
    }
}

void AnimationState::StartAnimation() {
    m_paused = false;
}

void AnimationState::PauseAnimation() {
    m_paused = true;
}

bool AnimationState::IsComplete() {
    return m_isComplete;
}

int AnimationState::GetAnimationIndex() {
    return m_index;
}

const AnimatedNode* FindAnimatedNode(Animation* animation, const char* NodeName) {
    for (unsigned int i = 0; i < animation->m_animatedNodes.size(); i++) {
        const AnimatedNode* animatedNode = &animation->m_animatedNodes[i];
        if (Util::StrCmp(animatedNode->m_nodeName, NodeName)) {
            return animatedNode;
        }
    }
    return nullptr;
}

void AnimationState::SetToBindPose() {
    m_index = -1;
}



void AnimationState::Update(int skinnedModelIndex, float deltaTime) {
    Animation* animation = AssetManager::GetAnimationByIndex(m_index, false);
    SkinnedModel* skinnedModel = AssetManager::GetSkinnedModelByIndex(skinnedModelIndex);
    if (!skinnedModel) {
        return;
    }
    // Ensure transforms match the number of joints
    m_globalNodeTransforms.resize(skinnedModel->m_nodes.size());

    // Update current animation time based on deltaTime
    float timeInTicks = 0;
    if (animation) {
        if (!m_paused) {
            m_currentTime += deltaTime * m_playbackParams.animationSpeed;
        }
        float duration = animation->m_duration / animation->m_ticksPerSecond;
        if (m_currentTime >= duration) {
            if (!m_loop) {
                m_currentTime = duration;
                m_paused = true;
                m_isComplete = true;
            }
            else {
                m_currentTime = 0;
            }
        }
        timeInTicks = GetTimeInTicks(m_currentTime);
    }

    // Traverse the tree
    for (int i = 0; i < skinnedModel->m_nodes.size(); i++) {
        glm::mat4 nodeTransformation = glm::mat4(1);
        std::string& nodeName = skinnedModel->m_nodes[i].name;

        // Interpolate the node transformation if it's animated
        if (animation) {
            const AnimatedNode* animatedNode = Util::FindAnimatedNode(animation, nodeName.c_str());
            if (animatedNode) {
                glm::vec3 translation;
                glm::quat rotation;
                glm::vec3 scale;
                Util::CalcInterpolatedPosition(translation, timeInTicks, animatedNode);
                Util::CalcInterpolatedRotation(rotation, timeInTicks, animatedNode);
                Util::CalcInterpolatedScale(scale, timeInTicks, animatedNode);
                nodeTransformation = glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation);
                nodeTransformation = glm::scale(nodeTransformation, scale);
            }
        }
        else {
            nodeTransformation = skinnedModel->m_nodes[i].inverseBindTransform;
        }

        // Calculate the world transform for this joint
        unsigned int parentIndex = skinnedModel->m_nodes[i].parentIndex;
        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : m_globalNodeTransforms[parentIndex].to_mat4();
        glm::mat4 GlobalTransformation = ParentTransformation * nodeTransformation;
        m_globalNodeTransforms[i] = AnimatedTransform(GlobalTransformation);
    }

    // Ease out
    if (m_easeOut) {
        m_easeOutTimer += deltaTime;
        m_easeOutTimer = std::clamp(m_easeOutTimer, 0.0f, 1.0f);
    }
    float normalizedTime = glm::clamp(m_easeOutTimer / m_playbackParams.easeOutDuration, 0.0f, 1.0f);
    float easedValue = Util::EaseOut(normalizedTime);
    m_blendFactor = m_playbackParams.blendWeight - easedValue;
}

float AnimationState::GetTimeInTicks(float currentTime) {
    Animation* animation = AssetManager::GetAnimationByIndex(m_index);
    if (animation) {
        float ticksPerSecond = animation->m_ticksPerSecond != 0 ? animation->m_ticksPerSecond : 25.0f;
        float timeInTicks = currentTime * ticksPerSecond;
        if (timeInTicks == animation->m_duration) {
            return timeInTicks;
        }
        else {
            timeInTicks = fmod(timeInTicks, animation->m_duration);
            timeInTicks = std::min(timeInTicks, animation->m_duration);
            return timeInTicks;
        }
    }
    else {
        return 0;
    }
}

int AnimationState::GetAnimationFrameNumber() {
    Animation* animation = AssetManager::GetAnimationByIndex(m_index);
    if (animation) {
        return  m_currentTime * animation->m_ticksPerSecond;
    }
    else {
        return 0;
    }
}

bool AnimationState::AnimationIsPastFrameNumber(int frameNumber) {
    return frameNumber < GetAnimationFrameNumber();
}

std::string AnimationState::GetDebugInfo() {
    std::string text = "Index: " + std::to_string(m_index) + "\n";
    if (m_index != -1) {
        Animation* animation = AssetManager::GetAnimationByIndex(m_index);
        text += "Name: " + animation->GetName() + "\n";
        text += "CurrentTime: " + std::to_string(m_currentTime) + "\n";
        text += "CurrentFrame: " + std::to_string(GetAnimationFrameNumber()) + "\n";
        text += "Duration: " + std::to_string(animation->m_duration) + "\n";
        text += "IsComplete: " + std::to_string(IsComplete()) + "\n";
        text += "Loop: " + std::to_string(m_loop) + "\n";
        text += "ForceStop: " + std::to_string(m_forceStop) + "\n";
        text += "Paused: " + std::to_string(m_paused) + "\n";
        text += "BlendWeight: " + std::to_string(m_playbackParams.blendWeight) + "\n";
        text += "BlendFactor: " + std::to_string(m_blendFactor) + "\n";
        text += "m_easeOutTimer: " + std::to_string(m_easeOutTimer) + "\n";
    }
    return text;
}

bool AnimationState::AwaitingRemoval() {
    if (m_playbackParams.removeAnimationStateWhenComplete&& IsComplete()) {
        return true;
    }
    if (m_forceStop) {         
        return true;
    }
    if (m_blendFactor == 0) {
        return true;
    }
    return false;
}

void AnimationState::ForceStop() {
    m_forceStop = true;
}

void AnimationState::ForceEaseOut() {
    m_easeOut = true;
}

