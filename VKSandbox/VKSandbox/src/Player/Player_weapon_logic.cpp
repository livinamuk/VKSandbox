#include "Player.h"
#include "Core/Audio.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Util.h"

// GET ME OUT OF HERE
#include "World/World.h"
// GET ME OUT OF HERE

void Player::UpdateWeaponLogic() {
    if (!HasControl()) return;

    if (PressedNextWeapon()) {
        NextWeapon();
    }

    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetCurrentWeaponState();

    if (!viewWeapon) return;
    if (!weaponInfo) return;

    switch (GetCurrentWeaponType()) {
    case WeaponType::MELEE:     UpdateMeleeLogic();      break;
    case WeaponType::PISTOL:    UpdateGunLogic();        break;
    case WeaponType::AUTOMATIC: UpdateGunLogic();        break;
    case WeaponType::SHOTGUN:   UpdateShotgunGunLogic(); break;
    }

    // Need to initiate draw animation?
    if (GetCurrentWeaponAction() == WeaponAction::DRAW_BEGIN) {

        // Shotgun may need pump
        if (GetCurrentWeaponType() == WeaponType::SHOTGUN && !IsShellInShotgunChamber() && weaponState->ammoInMag > 0) {
            viewWeapon->PlayAnimation(weaponInfo->animationNames.shotgunDrawPump, AnimationPlaybackParams::GetDefaultPararms());
            weaponState->shotgunAwaitingPumpAudio = true;
            weaponState->shotgunRequiresPump = true;
            m_weaponAction = DRAWING_WITH_SHOTGUN_PUMP;
        }
        // Initiate regular draw
        else {
            viewWeapon->PlayAnimation(weaponInfo->animationNames.draw, AnimationPlaybackParams::GetDefaultPararms());
            m_weaponAction = DRAWING;
        }
    }

    // Finished drawing? Return to idle
    if (GetCurrentWeaponAction() == WeaponAction::DRAWING && viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.draw) ||
        GetCurrentWeaponAction() == WeaponAction::DRAWING_WITH_SHOTGUN_PUMP && viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.shotgunDrawPump)) {
        m_weaponAction = WeaponAction::IDLE;
    }

    // In idle? Then play idle or walk if moving
    if (GetCurrentWeaponAction() == WeaponAction::IDLE) {
        const std::string& animName = IsMoving() ? weaponInfo->animationNames.walk : weaponInfo->animationNames.idle;
        viewWeapon->PlayAndLoopAnimation(animName, AnimationPlaybackParams::GetDefaultLoopingPararms());
    }

    // Everything done? Go to idle
    if (ViewModelAnimationsCompleted()) {
        m_weaponAction = WeaponAction::IDLE;
    }
}

void Player::GiveDefaultLoadout() {
    GiveWeapon("Knife");
    GiveWeapon("SPAS");    
    GiveAmmo("Shotgun", 80); 
    GiveAmmo("Tokarev", 200);
}

void Player::NextWeapon() {
    m_currentWeaponIndex++;
    if (m_currentWeaponIndex == m_weaponStates.size()) {
        m_currentWeaponIndex = 0;
    }
    while (!m_weaponStates[m_currentWeaponIndex].has) {
        m_currentWeaponIndex++;
        if (m_currentWeaponIndex == m_weaponStates.size()) {
            m_currentWeaponIndex = 0;
        }
    }
    Audio::PlayAudio("NextWeapon.wav", 0.5f);
    SwitchWeapon(m_weaponStates[m_currentWeaponIndex].name, DRAW_BEGIN);
}

void Player::SwitchWeapon(const std::string& name, WeaponAction weaponAction) {
    WeaponState* state = GetWeaponStateByName(name);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();

    if (!state) return;
    if (!weaponInfo) return;
    if (!viewWeapon) return;

    for (int i = 0; i < m_weaponStates.size(); i++) {
        if (m_weaponStates[i].name == name) {
            m_currentWeaponIndex = i;
        }
    }
    viewWeapon->SetName(weaponInfo->name);
    viewWeapon->SetSkinnedModel(weaponInfo->modelName);
    viewWeapon->EnableDrawingForAllMesh();

    // Is it gold?
    weaponInfo->isGold ? viewWeapon->MakeGold() : viewWeapon->MakeNotGold();

    // Set animation
    if (weaponAction == SPAWNING) {
        //    viewWeaponAnimatedGameObject->PlayAnimation(weaponInfo->animationNames.spawn, 1.0f);
    }
    if (weaponAction == DRAW_BEGIN) {
        //    viewWeaponAnimatedGameObject->PlayAnimation(weaponInfo->animationNames.draw, 1.0f);
    }
    // Set materials
    for (auto& it : weaponInfo->meshMaterials) {
        viewWeapon->SetMeshMaterialByMeshName(it.first, it.second);
    }
    // Set materials by index
    for (auto& it : weaponInfo->meshMaterialsByIndex) {
        viewWeapon->SetMeshMaterialByMeshIndex(it.first, it.second);
    }
    // Hide mesh
    for (auto& meshName : weaponInfo->hiddenMeshAtStart) {
        viewWeapon->DisableDrawingForMeshByMeshName(meshName);
    }
    m_weaponAction = weaponAction;

    Audio::PlayAudio("NextWeapon.wav", 0.5f);
}

WeaponType Player::GetCurrentWeaponType() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (weaponInfo) {
        return weaponInfo->type;
    }
}

WeaponAction Player::GetCurrentWeaponAction() {
    return m_weaponAction;
}

WeaponInfo* Player::GetCurrentWeaponInfo() {
    return WeaponManager::GetWeaponInfoByName(m_weaponStates[m_currentWeaponIndex].name);;
}

void Player::GiveWeapon(const std::string& name) {
    WeaponState* state = GetWeaponStateByName(name);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    if (state && weaponInfo) {
        state->has = true;
        state->ammoInMag = weaponInfo->magSize;
    }
}

void Player::GiveAmmo(const std::string& name, int amount) {
    AmmoState* state = GetAmmoStateByName(name);
    if (state) {
        state->ammoOnHand += amount;
    }
}

void Player::GiveRedDotToWeapon(const std::string& name) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    WeaponState* state = GetWeaponStateByName(name);
    if (state && weaponInfo && weaponInfo->type == WeaponType::PISTOL) {
        state->hasScope = true;
    }
}

void Player::GiveSilencerToWeapon(const std::string& name) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(name);
    WeaponState* state = GetWeaponStateByName(name);
    if (state && weaponInfo && weaponInfo->type == WeaponType::PISTOL) {
        state->hasSilencer = true;
    }
}

WeaponState* Player::GetWeaponStateByName(const std::string& name) {
    for (int i = 0; i < m_weaponStates.size(); i++) {
        if (m_weaponStates[i].name == name) {
            return &m_weaponStates[i];
        }
    }
    return nullptr;
}

AmmoState* Player::GetAmmoStateByName(const std::string& name) {
    for (int i = 0; i < m_ammoStates.size(); i++) {
        if (m_ammoStates[i].name == name) {
            return &m_ammoStates[i];
        }
    }
    return nullptr;
}

AmmoState* Player::GetCurrentAmmoState() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return nullptr;

    return GetAmmoStateByName(weaponInfo->ammoType);
}

AmmoInfo* Player::GetCurrentAmmoInfo() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return nullptr;

    return WeaponManager::GetAmmoInfoByName(weaponInfo->ammoType);
}

WeaponState* Player::GetCurrentWeaponState() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    if (!weaponInfo) return nullptr;

    return GetWeaponStateByName(weaponInfo->name);
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
    AmmoState* ammoState = GetCurrentAmmoState();
    if (!ammoState) return 0;

    return ammoState->ammoOnHand;
}

void Player::SpawnMuzzleFlash(float speed, float scale) {
    m_muzzleFlash.SetSpeed(55.0f);
    m_muzzleFlash.SetScale(glm::vec3(scale));
    m_muzzleFlash.SetTime(0.0f);
    m_muzzleFlash.EnableRendering();
    m_muzzleFlash.SetRotation(glm::vec3(0.0f, 0.0f, Util::RandomFloat(0, HELL_PI * 2)));
}

void Player::SpawnCasing(AmmoInfo* ammoInfo, bool alternateAmmo) {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (!Util::StrCmp(ammoInfo->casingModelName, UNDEFINED_STRING)) {
        BulletCasingCreateInfo createInfo;
        createInfo.modelIndex = AssetManager::GetModelIndexByName(ammoInfo->casingModelName);
        createInfo.materialIndex = AssetManager::GetMaterialIndexByName(ammoInfo->casingMaterialName);
        createInfo.position = viewWeapon->GetBoneWorldPosition(weaponInfo->casingEjectionBoneName);;
        createInfo.rotation.y = m_camera.GetYaw() + (HELL_PI * 0.5f);
        createInfo.force = glm::normalize(GetCameraRight() + glm::vec3(0.0f, Util::RandomFloat(0.7f, 0.9f), 0.0f)) * glm::vec3(0.00315f * weaponInfo->casingEjectionForce);

        createInfo.position += GetCameraForward() * glm::vec3(0.25f);
        createInfo.position += GetCameraRight() * glm::vec3(0.05f);
        createInfo.position += GetCameraUp() * glm::vec3(-0.025f);

        if (alternateAmmo) {
            createInfo.materialIndex = AssetManager::GetMaterialIndexByName("ShellGreen");
        }


        World::AddBulletCasing(createInfo);


    }
    else {
        std::cout << "Player::SpawnCasing(AmmoInfo* ammoInfo) failed to spawn a casing coz invalid casing model name in weapon info\n";
    }
}