#include "Player.h"
#include "HellDefines.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "BackEnd/BackEnd.h"
#include "Core/Audio.h"
#include "Core/Game.h"
#include "Core/Scene.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"

void Player::Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex) {
    m_position = position;
    m_camera.SetPosition(m_position + glm::vec3(0.0f, m_viewHeightStanding, 0.0f));
    m_camera.SetEulerRotation(rotation);
    m_viewportIndex = viewportIndex;

    // fix me: make it some unique ID
    m_playerIndex = viewportIndex;

    Scene::CreateAnimatedGameObject();
    m_viewWeaponAnimatedGameObjectIndex = Scene::GetAnimatedGameObjects().size() - 1;
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    viewWeapon->SetPlayerIndex(viewportIndex);
    viewWeapon->SetExclusiveViewportIndex(viewportIndex);

    Scene::CreateAnimatedGameObject();
    m_characterModelAnimatedGameObjectIndex = Scene::GetAnimatedGameObjects().size() - 1;
    AnimatedGameObject* characterModel = GetCharacterModelAnimatedGameObject();
    characterModel->SetPlayerIndex(viewportIndex);

    SpriteSheetObjectCreateInfo createInfo;
    createInfo.textureName = "MuzzleFlash_4x5";
    createInfo.loop = false;
    createInfo.billboard = true;
    createInfo.renderingEnabled = false;
    m_muzzleFlash.Init(createInfo);
    
    CreateCharacterController(m_position);

}

void Player::Update(float deltaTime) {
    if (Editor::IsOpen()) {
        return;
    }

    if (IsAwaitingSpawn()) Respawn();
    
    UpdateMovement(deltaTime);
    UpdateCharacterController();
    UpdateHeadBob(deltaTime);
    UpdateBreatheBob(deltaTime);
    UpdateCamera(deltaTime);
    UpdateWeaponLogic();       
    UpdateViewWeapon(deltaTime);
    UpdateSpriteSheets(deltaTime);
    UpdateUI();
    if (Input::KeyPressed(HELL_KEY_K)) {
        m_awaitingSpawn = true;
    }

    if (m_infoTextTimer > 0) {
        m_infoTextTimer -= deltaTime;
    }
    else {
        m_infoTextTimer = 0;
        m_infoText = "";
    }

    if (Input::KeyPressed(HELL_KEY_N) && m_viewportIndex == 0) {

        Transform transform;
        transform.position = m_position + m_camera.GetForward();

        PhysicsFilterData filterData;
        filterData.raycastGroup = RAYCAST_DISABLED;
        filterData.collisionGroup = CollisionGroup::GENERIC_BOUNCEABLE;
        filterData.collidesWith = (CollisionGroup)(ENVIROMENT_OBSTACLE | GENERIC_BOUNCEABLE | RAGDOLL);

        PxShape* pxShape = Physics::CreateBoxShape(1.0f, 1.0f, 1.0f, Transform());
        PxRigidDynamic* pxRigid = Physics::CreateRigidDynamic(transform, filterData, pxShape, Transform());

    }



}

void Player::Respawn() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    //viewWeapon->SetSkinnedModel("Glock");
    //viewWeapon->PlayAndLoopAnimation("Glock_Reload");
    //viewWeapon->SetSkinnedModel("SPAS");
    //viewWeapon->PlayAndLoopAnimation("SPAS_Reload2Shells");
    //viewWeapon->SetSkinnedModel("Knife");
    //viewWeapon->PlayAndLoopAnimation("Knife_Idle");

    m_weaponStates.clear();
    for (int i = 0; i < WeaponManager::GetWeaponCount(); i++) {
        WeaponState& state = m_weaponStates.emplace_back();
        state.name = WeaponManager::GetWeaponInfoByIndex(i)->name;
        state.has = false;
        state.ammoInMag = 0;
    }

    m_ammoStates.clear();
    for (int i = 0; i < WeaponManager::GetAmmoTypeCount(); i++) {
        AmmoState& state = m_ammoStates.emplace_back();
        state.name = WeaponManager::GetAmmoInfoByIndex(i)->name;
        state.ammoOnHand = 0;
    }

    GiveDefaultLoadout();
    //SwitchWeapon("Knife", WeaponAction::DRAW_BEGIN);

    if (m_viewportIndex == 0) {
        SwitchWeapon("SPAS", WeaponAction::DRAW_BEGIN);
    }
    else {
        SwitchWeapon("Knife", WeaponAction::DRAW_BEGIN);
    }
        
    //if (_characterController) {
    //    PxExtendedVec3 globalPose = PxExtendedVec3(spawnPoint.position.x, spawnPoint.position.y, spawnPoint.position.z);
    //    _characterController->setFootPosition(globalPose);
    //}
    //_position = spawnPoint.position;
    //_rotation = spawnPoint.rotation;

    if (Game::GetTotalTime() > 1.0) {
        Audio::PlayAudio("Glock_Equip.wav", 0.5f);
    }
    m_awaitingSpawn = false;
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

bool Player::ViewportIsVisible() {
    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (!viewport) {
        return false;
    }
    else {
        return viewport->IsVisible();
    }
}

bool Player::ViewModelAnimationsCompleted() {
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    if (!viewWeapon) {
        std::cout << "WARNING!!! Player::ViewModelAnimationsCompleted() failed coz viewWeapon was nullptr\n";
        return true;
    }
    for (AnimationState& animationState : viewWeapon->m_animationLayer.m_animationStates) {
        if (!animationState.IsComplete()) {
            return false;
        }
    }
    return true;
}

float Player::GetWeaponAudioFrequency() {
    return m_weaponAudioFrequency;
}

glm::mat4 Player::GetViewWeaponCameraMatrix() {
    return m_viewWeaponCameraMatrix;
}


void Player::DisplayInfoText(const std::string& text) {
    m_infoTextTimer = 2.0f;
    m_infoText = text;
}