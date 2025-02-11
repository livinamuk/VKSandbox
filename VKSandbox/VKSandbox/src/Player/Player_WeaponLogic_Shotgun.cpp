#include "Player.h"
#include "Core/Audio.h"
#include "Input/Input.h"

void Player::UpdateShotgunGunLogic() {
    if (PressedFire() && CanFireShotgun()) {
        FireShotgun();
    }
    if (PressedReload() && CanReloadShotgun()) {
        ReloadShotgun();
    }

    UpdateShotgunReloadLogic();
    UpdatePumpAudio();

    if (Input::KeyPressed(HELL_KEY_I)) {
        WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        weaponState->ammoInMag--;
        weaponState->shotgunAwaitingPumpAudio = true;
    }
}

void Player::FireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    m_weaponAction = WeaponAction::FIRE;
    weaponState->ammoInMag--;
    weaponState->shellInShotgunChamber = false;

    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = weaponInfo->animationSpeeds.fire;

    // Needs to rack pump?
    if (weaponState->ammoInMag > 0) {
        weaponState->shotgunAwaitingPumpAudio = true;
        viewWeapon->PlayAnimation(weaponInfo->animationNames.fire[0], params);
    }
    // No pump rack
    else {
        weaponState->shotgunAwaitingPumpAudio = false;
        viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunFireEmpty, params);
    }

    Audio::PlayAudio(weaponInfo->audioFiles.fire[0], 1.0f, GetWeaponAudioFrequency());
}

void Player::ReloadShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadStart;

    viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadStart, params);
    m_weaponAction = RELOAD_SHOTGUN_BEGIN;
    weaponState->shotgunAwaitingFirstShellReload = true;
    weaponState->shotgunAwaitingSecondShellReload = true;
}

void Player::UpdateShotgunReloadLogic() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponAction weaponAction = GetCurrentWeaponAction();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);

    // Begin loading anim has complete, time to reload!
    if (ViewModelAnimationsCompleted() && weaponAction == WeaponAction::RELOAD_SHOTGUN_BEGIN ||
        ViewModelAnimationsCompleted() && weaponAction == WeaponAction::RELOAD_SHOTGUN_SINGLE_SHELL ||
        ViewModelAnimationsCompleted() && weaponAction == WeaponAction::RELOAD_SHOTGUN_DOUBLE_SHELL) {

        // Reload 1 shell ONLY if the shotty can only take 1, or you only have 1
        if (weaponState->ammoInMag == 7 || ammoState->ammoOnHand == 1) {

            AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadOneShell;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadOneShell, params);

            weaponState->shotgunAwaitingFirstShellReload = true;

            m_weaponAction = RELOAD_SHOTGUN_SINGLE_SHELL;
        }
        // Otherwise do the double reload
        else if (weaponState->ammoInMag <= 6) {
            AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadTwoShells;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadTwoShells, params);

            weaponState->shotgunAwaitingFirstShellReload = true;
            weaponState->shotgunAwaitingSecondShellReload = true;

            m_weaponAction = RELOAD_SHOTGUN_DOUBLE_SHELL;
        }
        // Otherwise reload is done, time to flip the gun back to idle
        else {
            AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadEnd;

            if (IsShellInShotgunChamber()) {
                m_weaponAction = RELOAD_SHOTGUN_END;
                weaponState->shotgunAwaitingPumpAudio = false;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEnd, params);
            }
            else {
                m_weaponAction = RELOAD_SHOTGUN_END_WITH_PUMP;
                weaponState->shotgunAwaitingPumpAudio = true;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEndPump, params);

            }
        }
    }

    // Cancel reload with no shell in chamber ?
    if (PressedFire() && !IsShellInShotgunChamber()) {
        if (weaponAction == RELOAD_SHOTGUN_BEGIN ||
            weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL ||
            weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL ||
            weaponAction == RELOAD_SHOTGUN_BEGIN ||
            weaponAction == RELOAD_SHOTGUN_BEGIN ||
            weaponAction == RELOAD_SHOTGUN_BEGIN) {
            AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
            weaponState->shotgunAwaitingPumpAudio = true;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEndPump, params);
            m_weaponAction = RELOAD_SHOTGUN_END_WITH_PUMP;
        }
    }

    // Audio
    if (weaponAction == WeaponAction::RELOAD_SHOTGUN_SINGLE_SHELL) {
        if (weaponState->shotgunAwaitingFirstShellReload && viewWeapon->AnimationIsPastFrameNumber(7)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingFirstShellReload = false;
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
        }
    }

    if (weaponAction == WeaponAction::RELOAD_SHOTGUN_DOUBLE_SHELL) {
        if (weaponState->shotgunAwaitingFirstShellReload && viewWeapon->AnimationIsPastFrameNumber(7)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingFirstShellReload = false;
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
        }
        if (weaponState->shotgunAwaitingSecondShellReload && viewWeapon->AnimationIsPastFrameNumber(19)) {
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingSecondShellReload = false;
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
        }
    }
}

void Player::UpdatePumpAudio() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    auto PlayPumpAudioIfNeeded = [&](WeaponAction action, int frame) {
        if (GetCurrentWeaponAction() == action &&
            viewWeapon->AnimationIsPastFrameNumber(frame) &&
            weaponState->shotgunAwaitingPumpAudio) {

            Audio::PlayAudio(weaponInfo->audioFiles.shotgunPump, 1.0f, GetWeaponAudioFrequency());
            weaponState->shotgunAwaitingPumpAudio = false;
            weaponState->shellInShotgunChamber = true;
        }
    };

    PlayPumpAudioIfNeeded(WeaponAction::FIRE, 3);
    PlayPumpAudioIfNeeded(WeaponAction::RELOAD_SHOTGUN_END_WITH_PUMP, 3);
    PlayPumpAudioIfNeeded(WeaponAction::DRAWING_WITH_SHOTGUN_PUMP, 6);
}

bool Player::CanFireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    if (weaponState->ammoInMag > 0 && weaponState->shellInShotgunChamber) {
        return (
                weaponAction == IDLE ||
                weaponAction == RELOAD_SHOTGUN_BEGIN ||
                weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL ||
                weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL ||
                weaponAction == RELOAD_SHOTGUN_END ||
                weaponAction == RELOAD_SHOTGUN_END_WITH_PUMP ||
                weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber(21));
    }
    else {
        return false;
    }
}

bool Player::CanReloadShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    WeaponAction weaponAction = GetCurrentWeaponAction();

    return (
        // If shotty aint full, and you have enough ammo
        weaponState->ammoInMag < weaponInfo->magSize && ammoState->ammoOnHand > 0 &&

        // And you are playing an acceptable animation to cancel
        weaponAction == IDLE ||
        weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber(22)
        );
}

bool Player::IsShellInShotgunChamber() {
    WeaponState* weaponState = GetCurrentWeaponState();
    if (!weaponState) return false;

    if (weaponState && GetCurrentWeaponType() == WeaponType::SHOTGUN) {
        return weaponState->shellInShotgunChamber;
    }
}

bool Player::ShotgunRequiresPump() {
    WeaponState* weaponState = GetCurrentWeaponState();
    if (!weaponState) return false;

    if (weaponState && GetCurrentWeaponType() == WeaponType::SHOTGUN) {
        return weaponState->shotgunRequiresPump;
    }
}