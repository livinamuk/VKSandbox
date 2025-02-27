#include "Player.h"
#include "Core/Audio.h"

void Player::UpdateMeleeLogic(float deltaTime) {
    if (PressingFire() && CanFireMelee()) {
        FireMelee();
    }
}

void Player::FireMelee() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();

    if (weaponInfo->audioFiles.fire.size()) {
        int rand = std::rand() % weaponInfo->audioFiles.fire.size();
        Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f);
    }

    if (weaponInfo->animationNames.fire.size()) {
        int rand = std::rand() % weaponInfo->animationNames.fire.size();

        AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
        params.animationSpeed = weaponInfo->animationSpeeds.fire;

        viewWeapon->PlayAnimation(weaponInfo->animationNames.fire[rand], params);
    }

    m_weaponAction = WeaponAction::FIRE;
}

bool Player::CanFireMelee() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponAction weaponAction = GetCurrentWeaponAction();
    return (
        weaponAction == IDLE ||
        weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber(5)
    );
}