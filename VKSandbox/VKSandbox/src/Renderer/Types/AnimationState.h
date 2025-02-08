#pragma once
#include "HellTypes.h"
#include <string>
#include <vector>

enum class BlendAction {
    CONSTANT,
    EASE_OUT
};

struct AnimationPlaybackParams {
    BlendAction blendAction = BlendAction::CONSTANT;
    float animationSpeed = 1.0f;
    float blendWeight = 1.0f;
    float easeOutDuration = 1.0f;
    bool removeAnimationStateWhenComplete = false;
    bool removeOtherExistingAnimations = true;
    std::string animationName = "";

    static AnimationPlaybackParams GetDefaultPararms() {
        AnimationPlaybackParams params;    
        params.removeOtherExistingAnimations = true;
        params.removeAnimationStateWhenComplete = false;
        params.animationSpeed = 1.0f;
        return params;
    }
    static AnimationPlaybackParams GetDefaultLoopingPararms() {
        AnimationPlaybackParams params;
        params.removeOtherExistingAnimations = true;
        params.removeAnimationStateWhenComplete = false;
        params.animationSpeed = 1.0f;
        return params;
    }
};

struct AnimationState {
    // Methods
public:
    void PlayAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams);
    void PlayAndLoopAnimation(const std::string& animationName, const AnimationPlaybackParams& playbackParams);
    void StartAnimation();
    void PauseAnimation();
    void Update(int skinnedModelIndex, float deltaTime);
    void SetToBindPose();
    void ForceStop();
    void ForceEaseOut();
    bool IsComplete();
    bool AwaitingRemoval();
    int GetAnimationIndex();
    int GetAnimationFrameNumber();
    bool AnimationIsPastFrameNumber(int frameNumber);
    std::string GetDebugInfo();

private:
    float GetTimeInTicks(float currentTime);
    // Members
public:
    std::vector<AnimatedTransform> m_globalNodeTransforms;
    AnimationPlaybackParams m_playbackParams;
    float m_blendFactor = 0;
    bool m_loop = false;
    bool m_paused = false;
    bool m_easeOut = false;
    float m_easeOutTimer = 0.0f;
    int m_index = -1; // this was private but you moved it while WIP on the shark, you don't want this public
private:
    bool m_isComplete = false;
    float m_currentTime = 0;
    bool m_forceStop = 0;
};