#include "Player.h"
#include "Core/Game.h"

void Player::UpdateAudio() {

    // Footsteps
    if (m_bobOffsetY < -0.04f && !m_footstepPlayed && IsGrounded()) {
        Game::PlayFootstepOutdoorAudio();
        m_footstepPlayed = true;
    }
    if (m_bobOffsetY > 0.0f) {
        m_footstepPlayed = false;
    }

    // Landing on ground
    if (m_grounded && !m_groundedLastFrame) {
        Game::PlayFootstepOutdoorAudio();
        m_headBobTime = 0.0f;
    }
}