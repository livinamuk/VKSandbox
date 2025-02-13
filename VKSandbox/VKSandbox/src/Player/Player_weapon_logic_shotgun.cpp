#include "Player.h"
#include "Core/Audio.h"
#include "Input/Input.h"

void Player::UpdateShotgunGunLogic() {
    if (PressingFire() && CanFireShotgun()) {
        FireShotgun();
    }
    if (PressingFire() && CanDryFireShotgun()) {
        DryFireShotgun();
    }
    if (PressedReload() && CanReloadShotgun()) {
        ReloadShotgun();
    }
    if (PressedADS() && CanToggleShotgunAuto()) {
        ToggleAutoShotgun();
    }
    UpdateShotgunReloadLogic();
    UpdatePumpAudio();

    if (Input::KeyPressed(HELL_KEY_Y)) {
        SpawnCasing(GetCurrentAmmoInfo());
    }
    if (Input::KeyPressed(HELL_KEY_I)) {
        AmmoState* ammoState = GetCurrentAmmoState();
        WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        ammoState->ammoOnHand--;
    }
    if (Input::KeyPressed(HELL_KEY_O)) {
        AmmoState* ammoState = GetCurrentAmmoState();
        WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        ammoState->ammoOnHand++;
    }

}

void Player::ToggleAutoShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = weaponInfo->animationSpeeds.fire;
    viewWeapon->PlayAnimation(weaponInfo->animationNames.toggleAutoShotgun, params);

    m_weaponAction = WeaponAction::TOGGLING_AUTO;
    weaponState->shotgunInAutoMode = !weaponState->shotgunInAutoMode;
    Audio::PlayAudio("SPAS_AutoToggle_STOLEN_FROM_HALFLIFE.wav", 1.0f, GetWeaponAudioFrequency());

    //if (weaponState->shotgunInAutoMode) {
    //    DisplayInfoText("AUTO SHOTGUN ENABLED");
    //}
    //else {
    //    DisplayInfoText("AUTO SHOTGUN DISABLED");
    //}    
}

void Player::FireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoInfo* ammoInfo = GetCurrentAmmoInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);

    m_weaponAction = WeaponAction::FIRE;
    weaponState->ammoInMag--;
    weaponState->shotgunShellChambered = weaponState->shotgunInAutoMode && weaponState->ammoInMag > 0;

    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    params.animationSpeed = weaponInfo->animationSpeeds.fire;

    // Needs to rack pump?
    if (!weaponState->shotgunInAutoMode && weaponState->ammoInMag > 0) {
        weaponState->shotgunAwaitingPumpAudio = true;
        viewWeapon->PlayAnimation(weaponInfo->animationNames.fire[0], params);
    }
    // No pump rack
    else {
        weaponState->shotgunAwaitingPumpAudio = false;
        viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunFireNoPump, params);
    }
    SpawnMuzzleFlash(55.0f, 0.3f);
    SpawnCasing(ammoInfo);
    Audio::PlayAudio(weaponInfo->audioFiles.fire[0], 1.0f, GetWeaponAudioFrequency());
}

void Player::DryFireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    viewWeapon->PlayAnimation(weaponInfo->animationNames.dryFire, params);

    m_weaponAction = WeaponAction::DRY_FIRE;
    Audio::PlayAudio("Dry_Fire_HalfLife.wav", 1.0f, GetWeaponAudioFrequency());
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
        if (weaponState->ammoInMag == 7 && ammoState->ammoOnHand > 0 || 
            ammoState->ammoOnHand == 1) {

            AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadOneShell;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadOneShell, params);

            weaponState->shotgunAwaitingFirstShellReload = true;

            m_weaponAction = RELOAD_SHOTGUN_SINGLE_SHELL;
        }
        // Otherwise do the double reload
        else if (weaponState->ammoInMag <= 6 && ammoState->ammoOnHand >= 2) {
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
    if (PressedFire() || PressedADS()) {
        if (weaponAction == RELOAD_SHOTGUN_BEGIN ||
            weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL ||
            weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL ||
            weaponAction == RELOAD_SHOTGUN_BEGIN ||
            weaponAction == RELOAD_SHOTGUN_BEGIN ||
            weaponAction == RELOAD_SHOTGUN_BEGIN) {
            AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
            weaponState->shotgunAwaitingPumpAudio = true;

            if (IsShellInShotgunChamber()) {
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEnd, params);
                m_weaponAction = RELOAD_SHOTGUN_END;
            }
            else {
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEndPump, params);
                m_weaponAction = RELOAD_SHOTGUN_END_WITH_PUMP;
            }
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
            weaponState->shotgunShellChambered = true;
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
    if (weaponState->ammoInMag > 0 && weaponState->shotgunShellChambered) {
        return (
            weaponAction == IDLE ||
            weaponAction == FIRE && !weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber(21) ||
            weaponAction == FIRE && weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber(4));
    }
    else {
        return false;
    }
}

bool Player::CanDryFireShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    if (weaponState->ammoInMag == 0) {
        return (
            weaponAction == IDLE ||
            weaponAction == DRY_FIRE && viewWeapon->AnimationIsPastFrameNumber(5) ||
            weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber(17)
        );
    }
    else {
        return false;
    }
}

bool Player::CanToggleShotgunAuto() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    return (
        weaponAction == IDLE ||
        weaponAction == FIRE && !weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber(21) ||
        weaponAction == FIRE && weaponState->shotgunInAutoMode && viewWeapon->AnimationIsPastFrameNumber(4) ||
        weaponAction == TOGGLING_AUTO && viewWeapon->AnimationIsPastFrameNumber(2)
    );
}

bool Player::CanReloadShotgun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    WeaponAction weaponAction = GetCurrentWeaponAction();
    
    // If shotty aint full, and you have enough ammo
    if (weaponState->ammoInMag < weaponInfo->magSize && ammoState->ammoOnHand > 0) {
        return (
            // And you are playing an acceptable animation to cancel
            weaponAction == IDLE ||
            weaponAction == FIRE && viewWeapon->AnimationIsPastFrameNumber(22) ||
            weaponAction == DRY_FIRE && viewWeapon->AnimationIsPastFrameNumber(5) ||
            weaponAction == RELOAD_SHOTGUN_END && viewWeapon->AnimationIsPastFrameNumber(5) ||
            weaponAction == RELOAD_SHOTGUN_END_WITH_PUMP && viewWeapon->AnimationIsPastFrameNumber(5)
        );
    }
    return false;
}

bool Player::IsShellInShotgunChamber() {
    WeaponState* weaponState = GetCurrentWeaponState();
    if (!weaponState) return false;

    if (weaponState && GetCurrentWeaponType() == WeaponType::SHOTGUN) {
        return weaponState->shotgunShellChambered;
    }
}

bool Player::ShotgunRequiresPump() {
    WeaponState* weaponState = GetCurrentWeaponState();
    if (!weaponState) return false;

    if (weaponState && GetCurrentWeaponType() == WeaponType::SHOTGUN) {
        return weaponState->shotgunRequiresPump;
    }
}

