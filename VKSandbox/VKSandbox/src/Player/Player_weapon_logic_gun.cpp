#include "Player.h"
#include "Core/Audio.h"
#include "Viewport/ViewportManager.h"

void Player::UpdateGunLogic(float deltaTime) {
    if (PressingADS() && CanEnterADS()) {
        EnterADS();
    }
    if (!PressingADS() && CanLeaveADS()) {
        LeaveADS();
    }
    if (PressingFire() && CanFireGun()) {
        FireGun();
    }
    if (PressedReload() && CanReloadGun()) {
        ReloadGun();
    }
    UpdateGunReloadLogic();
    UpdateSlideLogic();
    UpdateADSLogic(deltaTime);
}

void Player::FireGun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoInfo* ammoInfo = GetCurrentAmmoInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        
    if (weaponInfo->hasADS && IsInADS()) {
        viewWeapon->PlayAnimation(weaponInfo->animationNames.adsFire, weaponInfo->animationSpeeds.adsFire);
        m_weaponAction = WeaponAction::ADS_FIRE;
    } else {
        viewWeapon->PlayAnimation(weaponInfo->animationNames.fire, weaponInfo->animationSpeeds.fire);
        m_weaponAction = WeaponAction::FIRE;
    }
    
    SpawnMuzzleFlash(55.0f, 0.3f);
    SpawnCasing(ammoInfo, weaponState->shotgunSlug);

    weaponState->ammoInMag--;

    int rand = std::rand() % weaponInfo->audioFiles.fire.size();
    Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f, GetWeaponAudioFrequency());
}

void Player::ReloadGun() {
    AnimationPlaybackParams params = AnimationPlaybackParams::GetDefaultPararms();
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (GetCurrentWeaponMagAmmo() == 0) {
        Audio::PlayAudio(weaponInfo->audioFiles.reloadEmpty, 0.7f);
        params.animationSpeed = weaponInfo->animationSpeeds.reloadempty;
        viewWeapon->PlayAnimation(weaponInfo->animationNames.reloadempty, params);
        m_weaponAction = RELOAD_FROM_EMPTY;
    }
    else {
        Audio::PlayAudio(weaponInfo->audioFiles.reload, 0.8f);
        params.animationSpeed = weaponInfo->animationSpeeds.reload;
        viewWeapon->PlayAnimation(weaponInfo->animationNames.reload, params);
        m_weaponAction = RELOAD;
    }
    weaponState->awaitingMagReload = true;
}

void Player::UpdateGunReloadLogic() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetCurrentAmmoState();

    if (m_weaponAction != RELOAD && m_weaponAction != RELOAD_FROM_EMPTY) {
        return;
    }

    int frameNumber = 0;
    switch (m_weaponAction) {
        case RELOAD:            frameNumber = weaponInfo->reloadMagInFrameNumber;       break;
        case RELOAD_FROM_EMPTY: frameNumber = weaponInfo->reloadEmptyMagInFrameNumber;  break;
        default: return;
    }

    if (weaponState->awaitingMagReload && viewWeapon->AnimationIsPastFrameNumber(frameNumber)) {
        int ammoToGive = std::min(weaponInfo->magSize - weaponState->ammoInMag, ammoState->ammoOnHand);
        weaponState->ammoInMag += ammoToGive;
        ammoState->ammoOnHand -= ammoToGive;
        weaponState->awaitingMagReload = false;
    }
}

void Player::UpdateSlideLogic() {
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetCurrentAmmoState();
    if (weaponInfo->emptyReloadRequiresSlideOffset && m_weaponAction != RELOAD_FROM_EMPTY && weaponState->ammoInMag == 0) {
        weaponState->requiresSlideOffset = true;
    }
    else {
        weaponState->requiresSlideOffset = false;
    }
}

void Player::UpdateADSLogic(float deltaTime) {
    // Zoom
    float zoomSpeed = 0.075f;
    if (m_weaponAction == ADS_IN || m_weaponAction == ADS_IDLE || m_weaponAction == ADS_FIRE) {
        m_cameraZoom -= zoomSpeed;
    }
    else {
        m_cameraZoom += zoomSpeed;
    }
    m_cameraZoom = std::max(0.575f, m_cameraZoom);
    m_cameraZoom = std::min(1.0f, m_cameraZoom);

    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    viewport->SetPerspective(m_cameraZoom, NEAR_PLANE, FAR_PLANE);
}


void Player::EnterADS() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    viewWeapon->PlayAnimation(weaponInfo->animationNames.adsIn, weaponInfo->animationSpeeds.adsIn);
    m_weaponAction = ADS_IN;
}

void Player::LeaveADS() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    viewWeapon->PlayAnimation(weaponInfo->animationNames.adsOut, weaponInfo->animationSpeeds.adsOut);
    m_weaponAction = ADS_OUT;
}

bool Player::CanReloadGun() {
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    AmmoState* ammoState = GetCurrentAmmoState();
    return (weaponState->ammoInMag < weaponInfo->magSize && ammoState->ammoOnHand > 0);
}

bool Player::CanFireGun() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    WeaponAction weaponAction = GetCurrentWeaponAction();
    if (weaponState->ammoInMag > 0) {
        return (
            weaponAction == IDLE              ||
            weaponAction == ADS_IDLE          ||
            weaponAction == ADS_FIRE          && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.adsFire) ||
            weaponAction == DRAWING           && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.draw) ||
            weaponAction == FIRE              && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.fire) ||
            weaponAction == RELOAD            && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.reload) ||
            weaponAction == RELOAD_FROM_EMPTY && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.reloadFromEmpty));
    }
    else {
        std::cout << "Cannot fire gun\n";
        return false;
    }
}

bool Player::CanEnterADS() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponState* weaponState = GetCurrentWeaponState();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();;

    if (weaponInfo->hasADS) {
        return (m_weaponAction != RELOAD && m_weaponAction != RELOAD_FROM_EMPTY && !IsInADS() ||
                m_weaponAction == RELOAD && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.reload) ||
                m_weaponAction == RELOAD_FROM_EMPTY && viewWeapon->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.reloadFromEmpty));
    }
}

bool Player::CanLeaveADS() {
    return (m_weaponAction == ADS_IN || m_weaponAction == ADS_IDLE || m_weaponAction == ADS_FIRE);
}

bool Player::IsInADS() {
    return (m_weaponAction == ADS_IN ||
            m_weaponAction == ADS_OUT ||
            m_weaponAction == ADS_IDLE ||
            m_weaponAction == ADS_FIRE);
}