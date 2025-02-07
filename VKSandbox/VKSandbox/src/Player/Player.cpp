#include "Player.h"
#include "HellDefines.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Core/Game.h"
#include "../Core/Scene.h"
#include "../Editor/Editor.h"
#include "../Input/Input.h"
#include "../BackEnd/BackEnd.h"

void Player::Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex) {
    m_position = position;
    m_camera.SetEulerRotation(rotation);
    m_viewportIndex = viewportIndex;

    // fix me: make it some unique ID
    m_playerIndex = viewportIndex;

    Scene::CreateAnimatedGameObject();
    m_viewWeaponAnimatedGameObjectIndex = Scene::GetAnimatedGameObjects().size() - 1;
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    viewWeapon->SetFlag(AnimatedGameObject::Flag::VIEW_WEAPON);
    viewWeapon->SetPlayerIndex(viewportIndex);

    Scene::CreateAnimatedGameObject();
    m_characterModelAnimatedGameObjectIndex = Scene::GetAnimatedGameObjects().size() - 1;
    AnimatedGameObject* characterModel = GetCharacterModelAnimatedGameObject();
    characterModel->SetFlag(AnimatedGameObject::Flag::CHARACTER_MODEL);
    characterModel->SetPlayerIndex(viewportIndex);
}

void Player::Update(float deltaTime) {
    if (IsAwaitingSpawn()) Respawn();
    
    UpdateMovement(deltaTime);
    UpdateCamera();
    UpdateViewWeapon();
    UpdateUI();
}

void Player::Respawn() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    viewWeapon->SetSkinnedModel("Glock");
    viewWeapon->PlayAndLoopAnimation("Glock_Reload");
    viewWeapon->SetSkinnedModel("SPAS");
    viewWeapon->PlayAndLoopAnimation("SPAS_Reload2Shells");

    // Load weapon states
    //m_weaponStates.clear();
    //for (int i = 0; i < WeaponManager::GetWeaponCount(); i++) {
    //    WeaponState& state = m_weaponStates.emplace_back();
    //    state.name = WeaponManager::GetWeaponInfoByIndex(i)->name;
    //    state.has = false;
    //    state.ammoInMag = 0;
    //}
    //// Load ammo states
    //m_ammoStates.clear();
    //for (int i = 0; i < WeaponManager::GetAmmoTypeCount(); i++) {
    //    AmmoState& state = m_ammoStates.emplace_back();
    //    state.name = WeaponManager::GetAmmoInfoByIndex(i)->name;
    //    state.ammoOnHand = 0;
    //}
    //
    //GiveDefaultLoadout();
    //SwitchWeapon("Glock", SPAWNING);
    ////    SwitchWeapon("GoldenGlock", SPAWNING);
    //
    //if (_characterController) {
    //    PxExtendedVec3 globalPose = PxExtendedVec3(spawnPoint.position.x, spawnPoint.position.y, spawnPoint.position.z);
    //    _characterController->setFootPosition(globalPose);
    //}
    //_position = spawnPoint.position;
    //_rotation = spawnPoint.rotation;
    //Audio::PlayAudio("Glock_Equip.wav", 0.5f);
    
    m_awaitingSpawn = false;
}

void Player::GiveDefaultLoadout() {
   GiveWeapon("Knife");
   GiveWeapon("Glock");
   GiveWeapon("GoldenGlock");
   GiveWeapon("Tokarev");
   GiveWeapon("AKS74U");
   GiveWeapon("P90");
   GiveWeapon("Shotgun");
   GiveWeapon("SPAS");
   
   GiveAmmo("Glock", 80);
   GiveAmmo("Tokarev", 200);
   GiveAmmo("AKS74U", 999999);
   GiveAmmo("Shotgun", 6666);
   
   GiveRedDotToWeapon("GoldenGlock");
   // GiveSilencerToWeapon("Glock");
}

void Player::EnableControl() {
    m_controlEnabled = true;
}
void Player::DisableControl() {
    m_controlEnabled = false;
}

const bool Player::IsAwaitingSpawn() {
    return m_awaitingSpawn;
}

const bool Player::HasControl() {
    return m_controlEnabled;
}

const bool Player::IsLocal() const {
    return m_viewportIndex != -1;
}

const bool Player::IsOnline() const {
    return m_viewportIndex == -1;
}

const glm::mat4& Player::GetProjectionMatrix() const {
    int width = BackEnd::GetCurrentWindowWidth();
    int height = BackEnd::GetCurrentWindowHeight();
    return glm::perspective(1.0f, float(width) / float(height), NEAR_PLANE, FAR_PLANE);
}

const glm::mat4& Player::GetViewMatrix() const {
    return m_camera.GetViewMatrix();
}

const glm::mat4& Player::GetInverseViewMatrix() const {
    return m_camera.GetInverseViewMatrix();
}

const glm::vec3& Player::GetCameraPosition() const {
    return m_camera.GetPosition();
}

const glm::vec3& Player::GetCameraRotation() const {
    return m_camera.GetEulerRotation();
}

const glm::vec3& Player::GetCameraForward() const {
    return m_camera.GetForward();
}

const glm::vec3& Player::GetCameraRight() const {
    return m_camera.GetRight();
}

const glm::vec3& Player::GetCameraUp() const {
    return m_camera.GetUp();
}

const int32_t Player::GetViewportIndex() const {
    return m_viewportIndex;
}

Camera& Player::GetCamera() {
    return m_camera;
}

AnimatedGameObject* Player::GetCharacterModelAnimatedGameObject() {
    return Scene::GetAnimatedGameObjectByIndex(m_characterModelAnimatedGameObjectIndex);
}

AnimatedGameObject* Player::GetViewWeaponAnimatedGameObject() {
    return Scene::GetAnimatedGameObjectByIndex(m_viewWeaponAnimatedGameObjectIndex);
}

bool Player::IsDead() {
    return false;
}