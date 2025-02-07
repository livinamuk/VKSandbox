#include "Player.h"
#include "Core/Audio.h"
#include "Core/Game.h"
#include "Core/Scene.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Util.h"


/*
 ▄█        ▄██████▄     ▄██████▄   ▄█   ▄████████
███       ███    ███   ███    ███ ███  ███    ███
███       ███    ███   ███    █▀  ███▌ ███    █▀
███       ███    ███  ▄███        ███▌ ███
███       ███    ███ ▀▀███ ████▄  ███▌ ███
███       ███    ███   ███    ███ ███  ███    █▄
███▌    ▄ ███    ███   ███    ███ ███  ███    ███
█████▄▄██  ▀██████▀    ████████▀  █▀   ████████▀  */


void Player::UpdateViewWeaponLogic(float deltaTime) {

    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
    AmmoInfo* ammoInfo = WeaponManager::GetAmmoInfoByName(weaponInfo->ammoType);
    AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);

    m_firedThisFrame = false;

    float weaponAudioFrequency = 1.0f;
    if (CameraIsUnderwater()) {
        weaponAudioFrequency = 0.4f;
    }

    /*
     █▀▄▀█ █▀▀ █   █▀▀ █▀▀
     █ ▀ █ █▀▀ █   █▀▀ █▀▀
     ▀   ▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ */

    if (weaponInfo->type == WeaponType::MELEE) {

        // Idle
        if (_weaponAction == IDLE) {
            if (Player::IsMoving()) {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.walk, params);
            }
            else {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.idle, params);
            }
        }
        // Draw
        if (_weaponAction == DRAW_BEGIN) {
            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.draw, params);
            _weaponAction = DRAWING;
        }
        // Drawing
        if (_weaponAction == DRAWING && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }
        // Fire
        if (PressedFire() && CanFire()) {
            if (_weaponAction == DRAWING ||
                _weaponAction == IDLE ||
                _weaponAction == FIRE && viewWeapon->AnimationIsPastPercentage(25.0f) ||
                _weaponAction == RELOAD && viewWeapon->AnimationIsPastPercentage(80.0f)) {
                _weaponAction = FIRE;

                if (weaponInfo->audioFiles.fire.size()) {
                    int rand = std::rand() % weaponInfo->audioFiles.fire.size();
                    Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f);
                }
                if (weaponInfo->animationNames.fire.size()) {
                    int rand = std::rand() % weaponInfo->animationNames.fire.size();

                    AnimationPlaybackParams params;
                    params.removeWhenComplete = true;
                    params.animationSpeed = weaponInfo->animationSpeeds.fire;

                    viewWeapon->PlayAnimation(weaponInfo->animationNames.fire[rand], params);
                }
                CheckForMeleeHit();
            }
        }
        if (_weaponAction == FIRE && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }
    }

    /*
    █▀█ ▀█▀ █▀▀ ▀█▀ █▀█ █   █▀▀     █   █▀█ █ █ ▀█▀ █▀█ █▄█ █▀█ ▀█▀ ▀█▀ █▀▀ █▀▀
    █▀▀  █  ▀▀█  █  █ █ █   ▀▀█   ▄▀    █▀█ █ █  █  █ █ █ █ █▀█  █   █  █   ▀▀█
    ▀   ▀▀▀ ▀▀▀  ▀  ▀▀▀ ▀▀▀ ▀▀▀   ▀     ▀ ▀ ▀▀▀  ▀  ▀▀▀ ▀ ▀ ▀ ▀  ▀  ▀▀▀ ▀▀▀ ▀▀▀ */

    if (weaponInfo->type == WeaponType::PISTOL || weaponInfo->type == WeaponType::AUTOMATIC) {
        if (!weaponState) {
            return;
        }
        if (HasControl()) {
            static float current = 0;
            constexpr float max = 0.0018f;
            constexpr float speed = 20.0f;
            float zoomSpeed = 0.075f;

            // Empty
            if (CanFire() && PressedFire() && weaponState->ammoInMag == 0) {
                Audio::PlayAudio("Dry_Fire.wav", 0.8f, weaponAudioFrequency);
            }

            if (_weaponAction == ADS_IN ||
                _weaponAction == ADS_IDLE ||
                _weaponAction == ADS_FIRE
                ) {
                current = Util::FInterpTo(current, max, deltaTime, speed);
                m_cameraZoom -= zoomSpeed;
            }
            else {
                current = Util::FInterpTo(current, 0, deltaTime, speed);
                m_cameraZoom += zoomSpeed;
            }
            // move the weapon down if you are in ads
            /*if (InADS()) {
                glm::vec3 offset = GetCameraUp() * current;
                glm::vec3 offset2 = GetCameraForward() * current;
                glm::vec3 position = Player::GetViewPos() - offset + offset2;
                viewWeapon->SetPosition(position);
            }*/
        }

        // ZOOM
        m_cameraZoom = std::max(0.575f, m_cameraZoom);
        m_cameraZoom = std::min(1.0f, m_cameraZoom);


        if (weaponInfo->name == "P90") {
            m_cameraZoom = std::max(0.775f, m_cameraZoom);
        }

        float adsInOutSpeed = 3.0f;

        // ADS in
        if (PressingADS() && CanEnterADS()) {
            _weaponAction = ADS_IN;

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = adsInOutSpeed;

            viewWeapon->PlayAnimation(weaponInfo->animationNames.adsIn, params);

            std::cout << "ENTERING ADS!\n";
        }
        // ADS in complete
        if (_weaponAction == ADS_IN && viewWeapon->IsAnimationComplete()) {

            AnimationPlaybackParams params;
            params.animationSpeed = 1.0f;

            viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.adsIdle, params);
            _weaponAction = ADS_IDLE;
        }
        // ADS out
        if (!PressingADS()) {

            if (_weaponAction == ADS_IN ||
                _weaponAction == ADS_IDLE) {
                _weaponAction = ADS_OUT;

                AnimationPlaybackParams params;
                params.animationSpeed = adsInOutSpeed;

                viewWeapon->PlayAnimation(weaponInfo->animationNames.adsOut, params);
            }
        }
        // ADS out complete
        if (_weaponAction == ADS_OUT && viewWeapon->IsAnimationComplete()) {

            AnimationPlaybackParams params;
            params.animationSpeed = 1.0f;

            viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.idle, params);
            _weaponAction = IDLE;
        }
        // ADS fire
        bool triggeredFire = (
            PressedFire() && weaponInfo->type == WeaponType::PISTOL && !weaponInfo->auomaticOverride ||
            PressingFire() && weaponInfo->type == WeaponType::PISTOL && weaponInfo->auomaticOverride ||
            PressingFire() && weaponInfo->type == WeaponType::AUTOMATIC
            );
        if (triggeredFire && CanFire() && InADS()) {
            // Has ammo
            if (weaponState->ammoInMag > 0) {
                _weaponAction = ADS_FIRE;
                if (weaponInfo->animationNames.adsFire.size()) {
                    int rand = std::rand() % weaponInfo->animationNames.adsFire.size();

                    AnimationPlaybackParams params;
                    params.removeWhenComplete = true;
                    params.animationSpeed = weaponInfo->animationSpeeds.adsFire;

                    viewWeapon->PlayAnimation(weaponInfo->animationNames.adsFire[rand], params);
                }
                if (!weaponState->hasSilencer) {
                    if (weaponInfo->audioFiles.fire.size()) {
                        int rand = std::rand() % weaponInfo->audioFiles.fire.size();
                        Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f, weaponAudioFrequency);
                    }
                }
                else {
                    Audio::PlayAudio("Silenced.wav", 1.0f, weaponAudioFrequency);
                }
                SpawnMuzzleFlash();
                SpawnBullet(0.05f * m_accuracyModifer, Weapon::AKS74U);
                SpawnCasing(ammoInfo);
                weaponState->ammoInMag--;
            }
        }
        // Finished ADS Fire
        if (_weaponAction == ADS_FIRE && viewWeapon->IsAnimationComplete()) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.0f;

            viewWeapon->PlayAnimation(weaponInfo->animationNames.idle, params);
            _weaponAction = ADS_IDLE;
        }
        // Not finished ADS Fire but player HAS LET GO OF RIGHT MOUSE
        if (_weaponAction == ADS_FIRE && !PressingADS()) {
            _weaponAction = ADS_OUT;

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = adsInOutSpeed;

            viewWeapon->PlayAnimation(weaponInfo->animationNames.adsOut, params);
        }
        // ADS walk
        if (_weaponAction == ADS_IDLE) {
            if (Player::IsMoving()) {
                AnimationPlaybackParams params;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.adsWalk, params);
            }
            else {
                AnimationPlaybackParams params;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.adsIdle, params);
            }
        }
        // Give reload ammo
        if (_weaponAction == RELOAD || _weaponAction == RELOAD_FROM_EMPTY) {

            int frameNumber = 0;

            if (_weaponAction == RELOAD) {
                frameNumber = weaponInfo->reloadMagInFrameNumber;
            }
            if (_weaponAction == RELOAD_FROM_EMPTY) {
                frameNumber = weaponInfo->reloadEmptyMagInFrameNumber;
            }
            if (_needsAmmoReloaded && viewWeapon->AnimationIsPastFrameNumber(frameNumber)) {
                int ammoToGive = std::min(weaponInfo->magSize - weaponState->ammoInMag, ammoState->ammoOnHand);
                weaponState->ammoInMag += ammoToGive;
                ammoState->ammoOnHand -= ammoToGive;
                _needsAmmoReloaded = false;
            }
        }
        // Revolver cocks
        if (weaponInfo->revolverCockFrameNumber != 0 &&
            m_revolverNeedsCocking &&
            viewWeapon->AnimationIsPastFrameNumber(weaponInfo->revolverCockFrameNumber)) {
            int rand = std::rand() % weaponInfo->audioFiles.revolverCocks.size();
            Audio::PlayAudio(weaponInfo->audioFiles.revolverCocks[rand], 1.0f, weaponAudioFrequency);
            m_revolverNeedsCocking = false;
        }

        // Fire (has ammo)
        if (triggeredFire && CanFire() && weaponState->ammoInMag > 0) {
            _weaponAction = FIRE;

            // Await cock
            if (weaponInfo->revolverCockFrameNumber != 0) {
                m_revolverNeedsCocking = true;
            }

            SpawnMuzzleFlash();
            SpawnBullet(0.05f * m_accuracyModifer, Weapon::AKS74U);
            SpawnCasing(ammoInfo);
            weaponState->ammoInMag--;

            if (!weaponState->hasSilencer) {
                if (weaponInfo->audioFiles.fire.size()) {
                    int rand = std::rand() % weaponInfo->audioFiles.fire.size();
                    Audio::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f, weaponAudioFrequency);
                }
            }
            else {
                Audio::PlayAudio("Silenced.wav", 1.0f, weaponAudioFrequency);
            }
            if (weaponInfo->animationNames.fire.size()) {
                int rand = std::rand() % weaponInfo->animationNames.fire.size();

                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.fire;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.fire[rand], params);
            }
        }
        // Reload
        if (PressedReload() && CanReload()) {

            // Revolver reload
            if (weaponInfo->relolverStyleReload) {
                _weaponAction = RELOAD_REVOLVER_BEGIN;

                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.revolverReloadBegin, params);

                _needsAmmoReloaded = true;
                weaponState->ammoInMag = 0;
                Audio::PlayAudio("Smith_ReloadBegin.wav", 1.0f);
                m_revolverReloadIterations = 0;
            }
            // Non revolver reloads
            else {
                if (GetCurrentWeaponMagAmmo() == 0) {
                    if (weaponInfo->animationNames.reloadempty.size()) {
                        int rand = std::rand() % weaponInfo->animationNames.reloadempty.size();

                        AnimationPlaybackParams params;
                        params.removeWhenComplete = true;
                        params.animationSpeed = weaponInfo->animationSpeeds.reloadempty;
                        viewWeapon->PlayAnimation(weaponInfo->animationNames.reloadempty[rand], params);
                    }

                    //  viewWeapon->PlayAnimation(weaponInfo->animationNames.reloadempty, 1.0f);
                    Audio::PlayAudio(weaponInfo->audioFiles.reloadEmpty, 0.7f);
                    _weaponAction = RELOAD_FROM_EMPTY;
                }
                else {
                    AnimationPlaybackParams params;
                    params.removeWhenComplete = true;
                    params.animationSpeed = weaponInfo->animationSpeeds.reload;
                    viewWeapon->PlayAnimation(weaponInfo->animationNames.reload, params);

                    Audio::PlayAudio(weaponInfo->audioFiles.reload, 0.8f);
                    _weaponAction = RELOAD;
                }
                _needsAmmoReloaded = true;
            }
        }

        // Revolver reload reloading logic
        if (_weaponAction == RELOAD_REVOLVER_BEGIN && viewWeapon->IsAnimationComplete()) {
            _weaponAction = RELOAD_REVOLVER_LOOP;

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.0f;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.revolverReloadLoop, params);

            m_revolverReloadIterations++;
        }

        // Add next revolver bullet
        if (_needsAmmoReloaded && _weaponAction == RELOAD_REVOLVER_LOOP && viewWeapon->AnimationIsPastFrameNumber(8)) {
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
            _needsAmmoReloaded = false;
            Audio::PlayAudio("Smith_ReloadLoop0.wav", 1.0f);
        }
        // Continue revolver reload loop
        if (_weaponAction == RELOAD_REVOLVER_LOOP && viewWeapon->IsAnimationComplete() && GetCurrentWeaponMagAmmo() < 6) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.0f;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.revolverReloadLoop, params);

            _needsAmmoReloaded = true;
        }
        // Trigger the end of the revolver reload loop
        if (_weaponAction == RELOAD_REVOLVER_LOOP && viewWeapon->IsAnimationComplete() && GetCurrentWeaponMagAmmo() >= 6) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.0f;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.revolverReloadEnd, params);

            _weaponAction = RELOAD_REVOLVER_END;
            Audio::PlayAudio("Smith_ReloadEnd.wav", 1.0f);
            std::cout << " Audio::PlayAudio(\"Smith_ReloadEnd.wav\", 1.0f);\n";
        }
        // End the revolver loop
        if (_weaponAction == RELOAD_REVOLVER_END && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }
        // Return to idle
        if (_weaponAction == RELOAD && viewWeapon->IsAnimationComplete() ||
            _weaponAction == RELOAD_FROM_EMPTY && viewWeapon->IsAnimationComplete() ||
            _weaponAction == SPAWNING && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }
        if (weaponInfo->name != "Smith & Wesson" && _weaponAction == FIRE && viewWeapon->AnimationIsPastPercentage(50.0f)) {
            _weaponAction = IDLE;
        }
        //Idle
        if (_weaponAction == IDLE) {
            if (Player::IsMoving()) {
                AnimationPlaybackParams params;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.walk, params);
            }
            else {
                AnimationPlaybackParams params;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.idle, params);
            }
        }
        // Draw
        if (_weaponAction == DRAW_BEGIN) {
            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.125f;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.draw, params);

            _weaponAction = DRAWING;
        }
        // Drawing
        if (_weaponAction == DRAWING && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }
    }
    // Pistol slide need sliding?
    weaponState->useSlideOffset = false;
    if (weaponInfo->type == WeaponType::PISTOL && _weaponAction != RELOAD_FROM_EMPTY && weaponState->ammoInMag == 0) {
        weaponState->useSlideOffset = true;
    }

    /*
    █▀▀ █ █ █▀█ ▀█▀ █▀▀ █ █ █▀█ █▀▀
    ▀▀█ █▀█ █ █  █  █ █ █ █ █ █ ▀▀█
    ▀▀▀ ▀ ▀ ▀▀▀  ▀  ▀▀▀ ▀▀▀ ▀ ▀ ▀▀▀ */

    if (weaponInfo->type == WeaponType::SHOTGUN) {

        // Empty
        if (CanFire() && PressedFire() && weaponState->ammoInMag == 0) {
            Audio::PlayAudio("Dry_Fire.wav", 0.8f, weaponAudioFrequency);
        }
        // Idle
        if (_weaponAction == IDLE) {
            if (Player::IsMoving()) {

                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.walk, params);
            }
            else {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAndLoopAnimation(weaponInfo->animationNames.idle, params);
            }
        }
        // Draw
        if (_weaponAction == DRAW_BEGIN) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.0f;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.draw, params);

            _weaponAction = DRAWING;
        }
        // Drawing
        if (_weaponAction == DRAWING && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }

        bool triggeredFire = (
            PressedFire() && weaponInfo->type == WeaponType::SHOTGUN && !weaponInfo->auomaticOverride ||
            PressingFire() && weaponInfo->type == WeaponType::SHOTGUN && weaponInfo->auomaticOverride);


        // Melee
        if (PressedMelee() && CanMelee()) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = 1.25f;

            viewWeapon->PlayAnimation(weaponInfo->animationNames.melee, params);
            std::string audioName = "Shotgun_Melee_Hit3.wav";
            Audio::PlayAudio(audioName, 1.0f);
            _weaponAction = MELEE;
        }
        if (_weaponAction == MELEE && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }
        // hit shit

        int frameNumber = GetViewWeaponAnimatedGameObject()->GetAnimationFrameNumber();
        int hitFrame = 9;
        int hitFrameWindow = 6;

        bool otherPlayerIsAlive = true;
        if (m_viewportIndex == 1 && Game::GetLocalPlayerByIndex(0)->IsDead()) {
            otherPlayerIsAlive = false;
        }
        if (m_viewportIndex == 0 && Game::GetLocalPlayerByIndex(1)->IsDead()) {
            otherPlayerIsAlive = false;
        }


        if (otherPlayerIsAlive && _weaponAction == MELEE && frameNumber > hitFrame && frameNumber < hitFrame + hitFrameWindow) {
            CheckForMeleeHit();
        }

        // Fire
        if (triggeredFire && CanFire()) {

            // Has ammo
            if (weaponState->ammoInMag > 0) {
                _weaponAction = FIRE;
                std::string aninName = "Shotgun_Fire";
                std::string audioName = "Shotgun_Fire.wav";

                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = 1.0f;
                viewWeapon->PlayAnimation(aninName, params);

                Audio::PlayAudio(audioName, 1.0f, weaponAudioFrequency);
                SpawnMuzzleFlash();
                for (int i = 0; i < 12; i++) {
                    SpawnBullet(0.1, Weapon::SHOTGUN);
                }
                m_shellEjectionState = ShellEjectionState::AWAITING_SHELL_EJECTION;


                // SpawnShotgunShell();
                weaponState->ammoInMag--;
            }
        }


        if (_weaponAction == FIRE && viewWeapon->AnimationIsPastPercentage(100.0f)) {
            _weaponAction = IDLE;
        }
        // Reload
        if (PressedReload() && CanReload()) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadStart;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadStart, params);
            _weaponAction = RELOAD_SHOTGUN_BEGIN;
        }

        // BEGIN RELOAD THING
        if (_weaponAction == RELOAD_SHOTGUN_BEGIN && viewWeapon->IsAnimationComplete()) {
            bool singleShell = false;
            if (weaponState->ammoInMag == 7 ||
                ammoState->ammoOnHand == 1) {
                singleShell = true;
            }

            // Single shell
            if (singleShell) {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadOneShell;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadOneShell, params);

                _weaponAction = RELOAD_SHOTGUN_SINGLE_SHELL;
            }
            // Double shell
            else {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadTwoShells;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadTwoShells, params);

                _weaponAction = RELOAD_SHOTGUN_DOUBLE_SHELL;
            }

            _needsShotgunFirstShellAdded = true;
            _needsShotgunSecondShellAdded = true;
        }
        // END RELOAD THING
        if (_weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL && viewWeapon->IsAnimationComplete() && GetCurrentWeaponMagAmmo() == SHOTGUN_AMMO_SIZE) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadEnd;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEnd, params);

            _weaponAction = RELOAD_SHOTGUN_END;
        }
        if (_weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL && viewWeapon->IsAnimationComplete() && GetCurrentWeaponMagAmmo() == SHOTGUN_AMMO_SIZE) {

            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadEnd;
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEnd, params);

            _weaponAction = RELOAD_SHOTGUN_END;
        }
        // CONTINUE THE RELOAD THING
        if (_weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL && viewWeapon->IsAnimationComplete()) {
            if (ammoState->ammoOnHand > 0) {

                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadOneShell;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadOneShell, params);

                _weaponAction = RELOAD_SHOTGUN_SINGLE_SHELL;
                _needsShotgunFirstShellAdded = true;
                _needsShotgunSecondShellAdded = true;
            }
            else {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadEnd;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadEnd, params);

                _weaponAction = RELOAD_SHOTGUN_END;
            }
        }
        if (_weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL && viewWeapon->IsAnimationComplete()) {
            bool singleShell = false;
            if (weaponState->ammoInMag == weaponInfo->magSize - 1 ||
                ammoState->ammoOnHand == 1) {
                singleShell = true;
            }
            // Single shell
            if (singleShell) {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadOneShell;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadOneShell, params);
                _weaponAction = RELOAD_SHOTGUN_SINGLE_SHELL;
            }
            // Double shell
            else {
                AnimationPlaybackParams params;
                params.removeWhenComplete = true;
                params.animationSpeed = weaponInfo->animationSpeeds.shotgunReloadTwoShells;
                viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunReloadTwoShells, params);
                _weaponAction = RELOAD_SHOTGUN_DOUBLE_SHELL;
            }
            _needsShotgunFirstShellAdded = true;
            _needsShotgunSecondShellAdded = true;
        }

        // Give ammo on reload
        if (_needsShotgunFirstShellAdded && _weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL && viewWeapon->AnimationIsPastPercentage(35.0f)) {
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
            _needsShotgunFirstShellAdded = false;
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, weaponAudioFrequency);
        }
        if (_needsShotgunFirstShellAdded && _weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL && viewWeapon->AnimationIsPastPercentage(28.0f)) {
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
            _needsShotgunFirstShellAdded = false;
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, weaponAudioFrequency);
        }
        if (_needsShotgunSecondShellAdded && _weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL && viewWeapon->AnimationIsPastPercentage(62.0f)) {
            weaponState->ammoInMag++;
            ammoState->ammoOnHand--;
            _needsShotgunSecondShellAdded = false;
            Audio::PlayAudio("Shotgun_Reload.wav", 1.0f, weaponAudioFrequency);
        }

        if (_weaponAction == FIRE && viewWeapon->IsAnimationComplete() ||
            _weaponAction == RELOAD_SHOTGUN_END && viewWeapon->IsAnimationComplete() ||
            _weaponAction == SPAWNING && viewWeapon->IsAnimationComplete()) {
            _weaponAction = IDLE;
        }


        if (m_shellEjectionState == ShellEjectionState::AWAITING_SHELL_EJECTION && _weaponAction == FIRE && viewWeapon->AnimationIsPastPercentage(35.0f)) {
            m_shellEjectionState = ShellEjectionState::IDLE;
            SpawnCasing(ammoInfo);
        }
    }


    if (m_firedThisFrame) {
        m_accuracyModifer = 1;
    }

    m_accuracyModifer = Util::FInterpTo(m_accuracyModifer, 0, deltaTime, 5.0);

    viewWeapon->Update(deltaTime);
}

/*
   ▄████████  ▄█    █▄     ▄████████    ▄████████ ▄██   ▄       ███        ▄█    █▄     ▄█  ███▄▄▄▄      ▄██████▄            ▄████████  ▄█          ▄████████    ▄████████
  ███    ███ ███    ███   ███    ███   ███    ███ ███   ██▄ ▀█████████▄   ███    ███   ███  ███▀▀▀██▄   ███    ███          ███    ███ ███         ███    ███   ███    ███
  ███    █▀  ███    ███   ███    █▀    ███    ███ ███▄▄▄███    ▀███▀▀██   ███    ███   ███▌ ███   ███   ███    █▀           ███    █▀  ███         ███    █▀    ███    █▀
 ▄███▄▄▄     ███    ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ ▀▀▀▀▀▀███     ███   ▀  ▄███▄▄▄▄███▄▄ ███▌ ███   ███  ▄███               ▄███▄▄▄     ███         ███         ▄███▄▄▄
▀▀███▀▀▀     ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ▄██   ███     ███     ▀▀███▀▀▀▀███▀  ███▌ ███   ███ ▀▀███ ████▄        ▀▀███▀▀▀     ███       ▀███████████ ▀▀███▀▀▀
  ███    █▄  ███    ███   ███    █▄  ▀███████████ ███   ███     ███       ███    ███   ███  ███   ███   ███    ███          ███    █▄  ███                ███   ███    █▄
  ███    ███ ███    ███   ███    ███   ███    ███ ███   ███     ███       ███    ███   ███  ███   ███   ███    ███          ███    ███ ███▌    ▄    ▄█    ███   ███    ███
  ██████████  ▀██████▀    ██████████   ███    ███  ▀█████▀     ▄████▀     ███    █▀    █▀    ▀█   █▀    ████████▀           ██████████ █████▄▄██  ▄████████▀    ██████████ */


bool Player::CanMelee() {
    return true;
}

bool Player::CanEnterADS() {

    AnimatedGameObject* viewWeapon = Scene::GetAnimatedGameObjectByIndex(m_viewWeaponAnimatedGameObjectIndex);
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();

    if (weaponInfo->name == "Tokarev") {
        return false;
    }
    if (weaponInfo->name == "AKS74U" && !InADS()) {
        return true;
    }

    if (weaponInfo->name != "P90" && weaponState && !weaponState->hasScope) {
        return false;
    }

    if (_weaponAction != RELOAD && _weaponAction != RELOAD_FROM_EMPTY && !InADS() ||
        _weaponAction == RELOAD && viewWeapon->AnimationIsPastPercentage(65.0f) ||
        _weaponAction == RELOAD_FROM_EMPTY && viewWeapon->AnimationIsPastPercentage(65.0f)) {
        return true;
    }
    else {
        return false;
    }
}

WeaponAction& Player::GetWeaponAction() {
    return _weaponAction;
}


bool Player::CanFire() {

    //if (Game::g_globalFadeOut <= 0.01f) {
    //    return false;
    //}

    AnimatedGameObject* viewWeaponGameObject = Scene::GetAnimatedGameObjectByIndex(m_viewWeaponAnimatedGameObjectIndex);
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (!HasControl() || IsDead()) {
        return false;
    }

    if (weaponInfo->type == WeaponType::PISTOL || weaponInfo->type == WeaponType::AUTOMATIC) {
        return (
            _weaponAction == IDLE ||
            _weaponAction == DRAWING && viewWeaponGameObject->AnimationIsPastPercentage(weaponInfo->animationCancelPercentages.draw) ||
            _weaponAction == FIRE && viewWeaponGameObject->AnimationIsPastPercentage(weaponInfo->animationCancelPercentages.fire) ||
            _weaponAction == RELOAD && viewWeaponGameObject->AnimationIsPastPercentage(weaponInfo->animationCancelPercentages.reload) ||
            _weaponAction == RELOAD_FROM_EMPTY && viewWeaponGameObject->AnimationIsPastPercentage(weaponInfo->animationCancelPercentages.reloadFromEmpty) ||
            _weaponAction == ADS_IDLE ||
            _weaponAction == ADS_FIRE && viewWeaponGameObject->AnimationIsPastPercentage(weaponInfo->animationCancelPercentages.adsFire)
            );
    }

    else if (weaponInfo->type == WeaponType::SHOTGUN) {
        return (_weaponAction == IDLE ||
            _weaponAction == DRAWING && viewWeaponGameObject->AnimationIsPastPercentage(50.0f) ||
            _weaponAction == FIRE && viewWeaponGameObject->AnimationIsPastPercentage(weaponInfo->animationCancelPercentages.fire) ||
            _weaponAction == RELOAD_SHOTGUN_BEGIN ||
            _weaponAction == RELOAD_SHOTGUN_END ||
            _weaponAction == RELOAD_SHOTGUN_SINGLE_SHELL ||
            _weaponAction == RELOAD_SHOTGUN_DOUBLE_SHELL ||
            _weaponAction == SPAWNING && viewWeaponGameObject->AnimationIsPastPercentage(5.0f)
            );
    }

    return true;

}


bool Player::InADS() {
    if (_weaponAction == ADS_IN ||
        _weaponAction == ADS_OUT ||
        _weaponAction == ADS_IDLE ||
        _weaponAction == ADS_FIRE)
        return true;
    else {
        return false;
    }
}


bool Player::CanReload() {

    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();
    AnimatedGameObject* viewWeaponModel = GetViewWeaponAnimatedGameObject();

    if (!HasControl()) {
        return false;
    }

    if (weaponState->ammoInMag < weaponInfo->magSize) {
        return true;
    }

    return false;

   
}

void Player::UpdateWeaponSway(float deltaTime) {

    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();
    AnimatedGameObject* viewWeaponModel = GetViewWeaponAnimatedGameObject();
    AnimatedGameObject* characterModel = GetCharacterModelAnimatedGameObject();

    if (HasControl()) {

        WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
        AnimatedGameObject* viewWeapon = Scene::GetAnimatedGameObjectByIndex(m_viewWeaponAnimatedGameObjectIndex);

        float xMax = 4.0;
        if (m_cameraZoom < 0.99f) {
            xMax = 2.0f;
        }

        if (weaponInfo && weaponInfo->name == "P90"
            // || weaponInfo && weaponInfo->name == "Tokarev"            // MAKE THIS A FLAG IN WEAPON INFO
            ) {
            xMax *= 2;
        }

        float SWAY_AMOUNT = 0.125f;
        float SMOOTH_AMOUNT = 4.0f;
        float SWAY_MIN_X = -2.25f;
        float SWAY_MAX_X = xMax;
        float SWAY_MIN_Y = -2;
        float SWAY_MAX_Y = 0.95f;
        float xOffset = -(float)InputMulti::GetMouseXOffset(m_mouseIndex);
        float yOffset = (float)InputMulti::GetMouseYOffset(m_mouseIndex);


        if (weaponInfo && weaponInfo->name == "Remington 870"
            // || weaponInfo && weaponInfo->name == "Tokarev"            // MAKE THIS A FLAG IN WEAPON INFO
            ) {
            xOffset *= -1;
        }


        float movementX = -xOffset * SWAY_AMOUNT;
        float movementY = -yOffset * SWAY_AMOUNT;

        movementX = std::min(movementX, SWAY_MAX_X);
        movementX = std::max(movementX, SWAY_MIN_X);
        movementY = std::min(movementY, SWAY_MAX_Y);
        movementY = std::max(movementY, SWAY_MIN_Y);

        if (InputMulti::RightMousePressed(m_mouseIndex)) {
            movementX *= 0.0f;
            movementY *= 0.0f;
        }
        // FIGURE THIS OUT
        // FIGURE THIS OUT
        // FIGURE THIS OUT
        // FIGURE THIS OUT
        // FIGURE THIS OUT
        // FIGURE THIS OUT

        m_weaponSwayTransform.position.x = Util::FInterpTo(m_weaponSwayTransform.position.x, movementX, deltaTime, SMOOTH_AMOUNT);
        m_weaponSwayTransform.position.y = Util::FInterpTo(m_weaponSwayTransform.position.y, movementY, deltaTime, SMOOTH_AMOUNT);
        m_weaponSwayMatrix = m_weaponSwayTransform.to_mat4();

        // Maybe you can combine the two below somehow by applying it at render time?
       //for (auto& transform : viewWeapon->_animatedTransforms.local) {
       //    transform = m_weaponSwayMatrix * transform;
       //}
       //for (auto& transform : viewWeapon->m_jointWorldMatrices) {
       //    //   transform.worldMatrix = _weaponSwayMatrix * transform.worldMatrix;
       //}
    }
}

void Player::DropWeapons() {

    //glm::vec3 spawnPos = GetFeetPosition() + glm::vec3(0, 1.5f, 0);
    //
    //for (int i = 0; i < WeaponManager::GetWeaponCount(); i++) {
    //
    //    WeaponState& weaponState = m_weaponStates[i];
    //    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByIndex(i);
    //
    //    if (weaponInfo->name == "Glock") {
    //        continue;
    //    }
    //
    //    if (weaponState.has) {
    //        if (weaponInfo->pickupConvexMeshModelName == UNDEFINED_STRING ||
    //            weaponInfo->modelName == UNDEFINED_STRING) {
    //            continue;
    //        }
    //        Scene::CreateGameObject();
    //        GameObject* weapon = Scene::GetGameObjectByIndex(Scene::GetGameObjectCount() - 1);
    //        weapon->SetPosition(spawnPos);
    //        weapon->SetRotationX(-1.7f);
    //        weapon->SetRotationY(0.0f);
    //        weapon->SetRotationZ(-1.6f);
    //        weapon->SetModel(weaponInfo->pickupModelName);
    //        weapon->SetName("PickUp");
    //        for (auto& it : weaponInfo->pickUpMeshMaterials) {
    //            weapon->SetMeshMaterialByMeshName(it.first, it.second);
    //        }
    //        //weapon->SetPickUpType(PickUpType::AKS74U);
    //        weapon->SetWakeOnStart(true);
    //        weapon->SetKinematic(false);
    //        weapon->AddCollisionShapeFromModelIndex(AssetManager::GetModelIndexByName(weaponInfo->pickupConvexMeshModelName));
    //        weapon->SetRaycastShapeFromModelIndex(AssetManager::GetModelIndexByName(weaponInfo->pickupModelName));
    //        weapon->SetModelMatrixMode(ModelMatrixMode::PHYSX_TRANSFORM);
    //        weapon->UpdateRigidBodyMassAndInertia(50.0f);
    //        weapon->DisableRespawnOnPickup();
    //        weapon->SetCollisionType(CollisionType::PICKUP);
    //        //weapon->DisableShadows();
    //        weapon->m_collisionRigidBody.SetGlobalPose(weapon->_transform.to_mat4());
    //    }
    //}
}

void Player::SwitchWeapon(std::string name, WeaponAction weaponAction) {

    WeaponState* state = GetWeaponStateByName(name);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    AnimatedGameObject* viewWeaponAnimatedGameObject = GetViewWeaponAnimatedGameObject();

    if (weaponInfo && state) {

        for (int i = 0; i < m_weaponStates.size(); i++) {
            if (m_weaponStates[i].name == name) {
                m_currentWeaponIndex = i;
            }
        }
        viewWeaponAnimatedGameObject->SetName(weaponInfo->name);
        viewWeaponAnimatedGameObject->SetSkinnedModel(weaponInfo->modelName);
        viewWeaponAnimatedGameObject->EnableDrawingForAllMesh();

        // Is it gold?
        if (weaponInfo->isGold) {
            viewWeaponAnimatedGameObject->MakeGold();
        }
        else {
            viewWeaponAnimatedGameObject->MakeNotGold();
        }
        // Set animation
        if (weaponAction == SPAWNING) {
            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            viewWeaponAnimatedGameObject->PlayAnimation(weaponInfo->animationNames.spawn, params);
        }
        if (weaponAction == DRAW_BEGIN) {
            AnimationPlaybackParams params;
            params.removeWhenComplete = true;
            viewWeaponAnimatedGameObject->PlayAnimation(weaponInfo->animationNames.draw, params);
        }
        // Set materials
        for (auto& it : weaponInfo->meshMaterials) {
            viewWeaponAnimatedGameObject->SetMeshMaterialByMeshName(it.first, it.second);
        }
        // Set materials by index
        for (auto& it : weaponInfo->meshMaterialsByIndex) {
            viewWeaponAnimatedGameObject->SetMeshMaterialByMeshIndex(it.first, it.second);
        }
        // Hide mesh
        for (auto& meshName : weaponInfo->hiddenMeshAtStart) {
            viewWeaponAnimatedGameObject->DisableDrawingForMeshByMeshName(meshName);
        }
        _weaponAction = weaponAction;
    }
}

void Player::SpawnMuzzleFlash() {
    _muzzleFlashTimer = 0;
    _muzzleFlashRotation = Util::RandomFloat(0, HELL_PI * 2);
}

int Player::GetCurrentWeaponMagAmmo() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (weaponInfo) {
        WeaponState* weaponState = GetWeaponStateByName(weaponInfo->name);
        if (weaponState) {
            return weaponState->ammoInMag;
        }
    }
    return 0;
}

int Player::GetCurrentWeaponTotalAmmo() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (weaponInfo) {
        AmmoState* ammoState = GetAmmoStateByName(weaponInfo->ammoType);
        if (ammoState) {
            return ammoState->ammoOnHand;
        }
    }
    return 0;
}

WeaponInfo* Player::GetCurrentWeaponInfo() {
    if (m_currentWeaponIndex < 0 || m_currentWeaponIndex > m_weaponStates.size()) {
        std::cout << "layer::GetCurrentWeaponInfo() failed because m_currentWeaponIndex was " << m_currentWeaponIndex << " but m_weaponStates.size() was " << m_weaponStates.size() << "\n";
        return nullptr;
    }
    return WeaponManager::GetWeaponInfoByName(m_weaponStates[m_currentWeaponIndex].name);;
}

WeaponState* Player::GetCurrentWeaponState() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (weaponInfo) {
        return GetWeaponStateByName(weaponInfo->name);
    }
    else {
        return nullptr;
    }
}

void Player::GiveWeapon(std::string name) {
    WeaponState* state = GetWeaponStateByName(name);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    if (state && weaponInfo) {
        state->has = true;
        state->ammoInMag = weaponInfo->magSize;
    }
}

void Player::GiveAmmo(std::string name, int amount) {
    AmmoState* state = GetAmmoStateByName(name);
    if (state) {
        state->ammoOnHand += amount;
    }
}

void Player::GiveRedDotToWeapon(std::string name) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    WeaponState* state = GetWeaponStateByName(name);
    if (state && weaponInfo && weaponInfo->type == WeaponType::PISTOL) {
        state->hasScope = true;
    }
}

void Player::GiveSilencerToWeapon(std::string name) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    WeaponState* state = GetWeaponStateByName(name);
    if (state && weaponInfo && weaponInfo->type == WeaponType::PISTOL) {
        state->hasSilencer = true;
    }
}

WeaponState* Player::GetWeaponStateByName(std::string name) {
    for (int i = 0; i < m_weaponStates.size(); i++) {
        if (m_weaponStates[i].name == name) {
            return &m_weaponStates[i];
        }
    }
    return nullptr;
}

AmmoState* Player::GetAmmoStateByName(std::string name) {
    for (int i = 0; i < m_ammoStates.size(); i++) {
        if (m_ammoStates[i].name == name) {
            return &m_ammoStates[i];
        }
    }
    return nullptr;
}



void Player::CheckForMeleeHit() {

    //const PxGeometry& overlapShape = _meleeHitCheckOverlapShape->getGeometry();
    //const PxTransform shapePose(_characterController->getActor()->getGlobalPose());
    //
    //OverlapReport overlapReport = Physics::OverlapTest(overlapShape, shapePose, CollisionGroup::GENERIC_BOUNCEABLE | RAGDOLL);
    //
    //
    //for (int i = 0; i < Game::GetPlayerCount(); i++) {
    //
    //    Player* otherPlayer = Game::GetPlayerByIndex(i);
    //
    //    if (otherPlayer == this) {
    //        continue;
    //    }
    //
    //    float distanceToOtherPlayer = glm::distance(otherPlayer->GetViewPos(), this->GetViewPos());
    //    if (distanceToOtherPlayer < 2) {
    //        otherPlayer->Kill();
    //        IncrementKillCount();
    //
    //
    //        glm::vec3 direction = glm::normalize(otherPlayer->GetViewPos() - this->GetViewPos());
    //
    //        // Apply force to their ragdoll
    //        AnimatedGameObject* hitCharacterModel = otherPlayer->GetCharacterAnimatedGameObject();
    //
    //        for (RigidComponent& rigidComponent : hitCharacterModel->m_ragdoll.m_rigidComponents) {
    //            float strength = 35;
    //            strength *= rigidComponent.mass * 1.5f;
    //            PxVec3 force = PxVec3(direction.x, direction.y, direction.z) * strength;
    //            rigidComponent.pxRigidBody->addForce(force);
    //        }
    //    }
    //}


}


void Player::SpawnBullet(float variance, Weapon type) {
   // _muzzleFlashCounter = 0.0005f;
   // Bullet bullet;
   // bullet.spawnPosition = m_camera.GetPosition();
   // bullet.type = type;
   // bullet.raycastFlags = _bulletFlags;// RaycastGroup::RAYCAST_ENABLED;
   // bullet.parentPlayersViewRotation = GetCameraRotation();
   // bullet.parentPlayerIndex = m_playerIndex;
   // m_firedThisFrame = true;
   //
   // WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
   // if (weaponInfo) {
   //     bullet.damage = weaponInfo->damage;
   // }
   //
   ////if (Game::g_liceneToKill) {
   ////    bullet.damage = 100;
   ////}
   //
   // glm::vec3 offset = glm::vec3(0);
   // offset.x = Util::RandomFloat(-(variance * 0.5f), variance * 0.5f);
   // offset.y = Util::RandomFloat(-(variance * 0.5f), variance * 0.5f);
   // offset.z = Util::RandomFloat(-(variance * 0.5f), variance * 0.5f);
   //
   // bullet.direction = (glm::normalize(GetCameraForward() + offset)) * glm::vec3(-1);
   // Scene::_bullets.push_back(bullet);
}

void Player::SpawnCasing(AmmoInfo* ammoInfo) {
    //AnimatedGameObject* viewWeaponGameObject = Scene::GetAnimatedGameObjectByIndex(m_viewWeaponAnimatedGameObjectIndex);
    //WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    //if (!weaponInfo) {
    //    return;
    //}
    //if (!Util::StrCmp(ammoInfo->casingModelName, UNDEFINED_STRING)) {
    //    int modelIndex = AssetManager::GetModelIndexByName(ammoInfo->casingModelName);
    //    int meshIndex = AssetManager::GetModelByIndex(modelIndex)->GetMeshIndices()[0];
    //    int materialIndex = AssetManager::GetMaterialIndex(ammoInfo->casingMaterialName);
    //
    //    static Mesh* casingMesh = AssetManager::GetMeshByIndex(meshIndex);
    //    static Model* casingModel = AssetManager::GetModelByIndex(modelIndex);
    //
    //    float width = std::abs(casingMesh->aabbMax.x - casingMesh->aabbMin.x);
    //    float height = std::abs(casingMesh->aabbMax.y - casingMesh->aabbMin.y);
    //    float depth = std::abs(casingMesh->aabbMax.z - casingMesh->aabbMin.z);
    //    glm::vec3 size = glm::vec3(width, height, depth);
    //
    //    Transform transform;
    //    transform.position = GetPistolCasingSpawnPostion();
    //    transform.rotation.x = HELL_PI * 0.5f;
    //    transform.rotation.y = _rotation.y + (HELL_PI * 0.5f);
    //
    //    PhysicsFilterData filterData;
    //    filterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
    //    filterData.collisionGroup = CollisionGroup::BULLET_CASING;
    //    filterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;
    //
    //    PxShape* shape = Physics::CreateBoxShape(size.x, size.y, size.z);
    //    PxRigidDynamic* body = Physics::CreateRigidDynamic(transform, filterData, shape);
    //
    //    PxVec3 force = Util::GlmVec3toPxVec3(glm::normalize(GetCameraRight() + glm::vec3(0.0f, Util::RandomFloat(0.7f, 0.9f), 0.0f)) * glm::vec3(0.00215f * weaponInfo->casingEjectionForce));
    //    body->addForce(force);
    //    body->setAngularVelocity(PxVec3(Util::RandomFloat(0.0f, 100.0f), Util::RandomFloat(0.0f, 100.0f), Util::RandomFloat(0.0f, 100.0f)));
    //    body->userData = nullptr;
    //    body->setName("BulletCasing");
    //
    //    BulletCasing bulletCasing;
    //    bulletCasing.m_modelIndex = modelIndex;
    //    bulletCasing.m_materialIndex = materialIndex;
    //    bulletCasing.m_pxRigidBody = body;
    //    bulletCasing.m_pxShape = shape;
    //    Scene::g_bulletCasings.push_back(bulletCasing);
    //}
    //else {
    //    std::cout << "Player::SpawnCasing() failed to spawn a casing coz invalid casing model name in weapon info\n";
    //}
}