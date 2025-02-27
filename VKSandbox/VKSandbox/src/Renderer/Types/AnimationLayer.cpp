#include "AnimationLayer.h"
#include "AssetManagement/AssetManager.h"
#include "Util.h"

void AnimationLayer::SetSkinnedModel(const std::string& skinnedModelName) {
    int index = AssetManager::GetSkinnedModelIndexByName(skinnedModelName);
    if (index != -1) {
        m_skinnedModelIndex = index;
        m_jointCount = AssetManager::GetSkinnedModelByIndex(m_skinnedModelIndex)->m_nodes.size();
        ClearAllAnimationStates();
    }
}

void AnimationLayer::Update(float deltaTime) {
    // Remove any animations awaiting removal
    for (int i = 0; i < m_animationStates.size(); i++) {
        if (m_animationStates[i].AwaitingRemoval()) {
            m_animationStates.erase(m_animationStates.begin() + i);
            i--;
        }
    }
    // Update each animation state
    for (AnimationState& animationState : m_animationStates) {
        animationState.Update(m_skinnedModelIndex, deltaTime);
        for (int i = 0; i < animationState.m_globalNodeTransforms.size(); i++) {
            m_globalBlendedNodeTransforms.push_back(animationState.m_globalNodeTransforms[i].to_mat4());
        }
    }
    // Calculate final blended transforms
    if (m_animationStates.size()) {
        m_globalBlendedNodeTransforms.clear();
        m_globalBlendedNodeTransforms.resize(m_jointCount);
        for (int i = 0; i < m_jointCount; i++) {
            std::vector<AnimatedTransform> animatedTransforms;
            std::vector<float> weights;
            for (AnimationState& animationState : m_animationStates) {
                animatedTransforms.push_back(animationState.m_globalNodeTransforms[i]);
                weights.push_back(animationState.m_blendFactor);
            }
            Util::NormalizeWeights(weights);
            m_globalBlendedNodeTransforms[i] = Util::BlendMultipleTransforms(animatedTransforms, weights).to_mat4();
        }
    }
    // If there are no animations then skin to bind pose
    else {
        SkinToBindPose();
    }
}

void AnimationLayer::PlayAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams) {
    int animationIndex = AssetManager::GetAnimationIndexByName(animationName);
    if (animationIndex != -1) {
        // Remove any existing animations if specified to do so
        if (playbackParams.removeOtherExistingAnimations) {
            m_animationStates.clear();
        }
        // If it is already playing, then restart the existing animation
        for (AnimationState& animationState : m_animationStates) {
            if (animationState.GetAnimationIndex() == animationIndex) {
                animationState.PlayAnimation(animationName, playbackParams);
                std::cout << "bailing because not sure\n";
                return;
            }
        }
        // If not, then add it
        AnimationState& animationState = m_animationStates.emplace_back();
        animationState.PlayAnimation(animationName, playbackParams);
    }
}

void AnimationLayer::PlayAndLoopAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams) {
    int animationIndex = AssetManager::GetAnimationIndexByName(animationName);

    if (animationIndex != -1) {
        // Remove any other existing animations if specified to do so
        if (playbackParams.removeOtherExistingAnimations) {
            for (int i = 0; i < m_animationStates.size(); i++) {
                if (m_animationStates[i].m_index != animationIndex) {
                    m_animationStates.erase(m_animationStates.begin() + i);
                    i--;
                }
            }
        }
        // rETHINK THIS
        for (AnimationState& animationState : m_animationStates) {
            if (animationState.GetAnimationIndex() == animationIndex) {
                //animationState.PlayAndLoopAnimation(animationName, playbackParams);
                return;
            }
        }
        // If not, then add it
        AnimationState& animationState = m_animationStates.emplace_back();
        animationState.PlayAndLoopAnimation(animationName, playbackParams);
    }
}

void AnimationLayer::SkinToBindPose() {
    if (m_skinnedModelIndex == -1) {
        std::cout << "AnimationLayer::SkinToBindPose() failed cause m_skinnedModelIndex was -1\n";
        return;
    }
    // Traverse the tree
    m_globalBlendedNodeTransforms.resize(m_jointCount);
    SkinnedModel* skinnedModel = AssetManager::GetSkinnedModelByIndex(m_skinnedModelIndex);
    for (int i = 0; i < skinnedModel->m_nodes.size(); i++) {
        glm::mat4 nodeTransformation = glm::mat4(1);
        nodeTransformation = skinnedModel->m_nodes[i].inverseBindTransform;
        unsigned int parentIndex = skinnedModel->m_nodes[i].parentIndex;
        glm::mat4 ParentTransformation = (parentIndex == -1) ? glm::mat4(1) : m_globalBlendedNodeTransforms[parentIndex];
        glm::mat4 GlobalTransformation = ParentTransformation * nodeTransformation;
        m_globalBlendedNodeTransforms[i] = AnimatedTransform(GlobalTransformation).to_mat4();
    }
}

void AnimationLayer::ClearAllAnimationStates() {
    m_animationStates.clear();
}

bool AnimationLayer::AllAnimationIsComplete() {
    std::cout << " THIS FUNCITON IS BROKEN COZ ANIMATION STATES CAN STAY AFTER COMPLETION!!!\n";
    return m_animationStates.empty();
}

void AnimationLayer::ForceStopAnimationStateByName(const std::string& animationName) {
    for (AnimationState& animationState : m_animationStates) {
        if (animationState.m_playbackParams.animationName == animationName) {
            animationState.ForceStop();
        }
    }
}
void AnimationLayer::ForceEaseOutAnimationStateByName(const std::string& animationName) {
    for (AnimationState& animationState : m_animationStates) {
        if (animationState.m_playbackParams.animationName == animationName) {
            animationState.ForceEaseOut();
        }
    }
}

void AnimationLayer::SetLoopStateByName(const std::string& animationName, bool loopState) {
    for (AnimationState& animationState : m_animationStates) {
        if (animationState.m_playbackParams.animationName == animationName) {
            animationState.m_loop = loopState;
        }
    }
}

void AnimationLayer::PauseAnimationStateByName(const std::string& animationName) {
    for (AnimationState& animationState : m_animationStates) {
        if (animationState.m_playbackParams.animationName == animationName) {
            animationState.m_paused = true;
        }
    }
}
