#include "Player.h"
#include "Core/Audio.h"
#include "Core/Game.h"
#include "Core/Scene.h"
#include "Input/Input.h"
#include "Input/InputMulti.h"
#include "Util.h"

void Player::UpdateWeaponLogic() {
    if (HasControl() && PressedNextWeapon()) {
        NextWeapon();
    }

    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (!viewWeapon) return;
    if (!weaponInfo) return;

    switch (GetCurrentWeaponType()) {
    case WeaponType::MELEE:     UpdateMeleeLogic();      break;
    case WeaponType::PISTOL:    UpdateGunLogic();        break;
    case WeaponType::AUTOMATIC: UpdateGunLogic();        break;
    case WeaponType::SHOTGUN:   UpdateShotgunGunLogic(); break;
    }

    if (GetCurrentWeaponAction() == WeaponAction::DRAW_BEGIN) {
        viewWeapon->PlayAnimation(weaponInfo->animationNames.draw, AnimationPlaybackParams::GetDefaultPararms());
        m_weaponAction = DRAWING;
    }

    if (GetCurrentWeaponAction() == WeaponAction::DRAWING) {
        if (viewWeapon->AnimationByNameIsComplete(weaponInfo->animationNames.draw)) {
            m_weaponAction = WeaponAction::IDLE;
        }
    }

    if (GetCurrentWeaponAction() == WeaponAction::IDLE) {
        const std::string& animName = IsMoving() ? weaponInfo->animationNames.walk : weaponInfo->animationNames.idle;
        viewWeapon->PlayAndLoopAnimation(animName, AnimationPlaybackParams::GetDefaultLoopingPararms());
    }

    if (ViewModelAnimationsCompleted()) {
        m_weaponAction = WeaponAction::IDLE;
    }
}

void Player::GiveDefaultLoadout() {
    GiveWeapon("Knife");
    GiveWeapon("SPAS"); 
    GiveAmmo("Shotgun", 6666);
    //GiveWeapon("Glock");
    //GiveWeapon("GoldenGlock");
    //GiveWeapon("Tokarev");
    //GiveWeapon("AKS74U");
    //GiveWeapon("P90");
    //GiveWeapon("Shotgun");
    //GiveAmmo("Glock", 80);
    //GiveAmmo("Tokarev", 200);
    //GiveAmmo("AKS74U", 999999);
    //
    //GiveRedDotToWeapon("GoldenGlock");
    // GiveSilencerToWeapon("Glock");
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

void Player::SwitchWeapon(std::string name, WeaponAction weaponAction) {
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