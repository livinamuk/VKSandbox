#pragma once
#include "HellTypes.h"
#include "Camera/Camera.h"
#include "Core/WeaponManager.h"
#include "Types/AnimatedGameObject.h"

struct Player {
    void Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex);
    void Update(float deltaTime);
    void Respawn();
    void EnableControl();
    void DisableControl();
    const bool IsAwaitingSpawn();
    const bool HasControl();
    const bool IsLocal() const;
    const bool IsOnline() const;
    const glm::mat4& GetProjectionMatrix() const;
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetInverseViewMatrix() const;
    const glm::vec3& GetCameraPosition() const;
    const glm::vec3& GetCameraRotation() const;
    const glm::vec3& GetCameraForward() const;
    const glm::vec3& GetCameraRight() const;
    const glm::vec3& GetCameraUp() const;
    const int32_t GetViewportIndex() const;
    Camera& GetCamera();
    AnimatedGameObject* GetCharacterModelAnimatedGameObject();
    AnimatedGameObject* GetViewWeaponAnimatedGameObject();

    void UpdateCamera();
    void UpdateViewWeapon();
    void UpdateMovement(float deltaTime);
    void UpdateUI();

    // Weapon shit
    int GetCurrentWeaponMagAmmo();
    int GetCurrentWeaponTotalAmmo();
    bool CanFire();
    bool CanReload();
    bool CanEnterADS();
    bool InADS(); 
    bool CanMelee();
    void UpdateViewWeaponLogic(float deltaTime);
    void GiveDefaultLoadout();
    void GiveWeapon(std::string name);
    void GiveAmmo(std::string name, int amount);
    void SwitchWeapon(std::string name, WeaponAction weaponAction);
    void GiveRedDotToWeapon(std::string name);
    void GiveSilencerToWeapon(std::string name);
    void DropWeapons();
    void UpdateWeaponSway(float deltaTime);
    void SpawnMuzzleFlash();
    WeaponAction& GetWeaponAction();
    WeaponInfo* GetCurrentWeaponInfo();
    WeaponState* GetWeaponStateByName(std::string name);
    WeaponState* GetCurrentWeaponState();
    AmmoState* GetAmmoStateByName(std::string name);

    //RenderItem CreateAttachmentRenderItem(WeaponAttachmentInfo* weaponAttachmentInfo, const char* boneName);
   
    void CheckForMeleeHit();

    // Water
    WaterState m_waterState;
    bool FeetEnteredUnderwater();
    bool FeetExitedUnderwater();
    bool CameraEnteredUnderwater();
    bool CameraExitedUnderwater();
    bool IsSwimming();
    bool IsWading();
    bool CameraIsUnderwater();
    bool FeetBelowWater();
    bool StartedSwimming();
    bool StoppedSwimming();
    bool StartingWading();
    bool StoppedWading();

    // State queries
    bool IsMoving();
    bool IsCrouching();
    bool IsDead();
    bool IsAlive();
    bool IsOverlappingLadder();
    bool IsAtShop();

    // Input
    int32_t GetKeyboardIndex();
    int32_t GetMouseIndex();
    void SetKeyboardIndex(int32_t index);
    void SetMouseIndex(int32_t index);
    bool PressingWalkForward();
    bool PressingWalkBackward();
    bool PressingWalkLeft();
    bool PressingWalkRight();
    bool PressingCrouch();
    bool PressedWalkForward();
    bool PressedWalkBackward();
    bool PressedWalkLeft();
    bool PressedWalkRight();
    bool PressedInteract();
    bool PressedReload();
    bool PressedFire();
    bool PressingFire();
    bool PresingJump();
    bool PressedCrouch();
    bool PressedNextWeapon();
    bool PressingADS();
    bool PressedADS();
    bool PressedEscape();
    bool PressedMelee();
    bool PressedFlashlight();


    void SpawnBullet(float variance, Weapon type);
    void SpawnCasing(AmmoInfo* ammoInfo);

    // Dev keys
    bool PressedFullscreen();
    bool PressedOne();
    bool PressedTwo();
    bool PressedThree();
    bool PressedFour();

private:
    glm::vec3 m_position = glm::vec3(0.0f);
    int m_revolverReloadIterations = 0;
    int m_currentWeaponIndex = 0;
    bool m_Moving = false;
    bool m_controlEnabled = true;
    bool m_awaitingSpawn = true;
    bool m_firedThisFrame = false;
    bool _needsAmmoReloaded = false;
    bool m_revolverNeedsCocking = false;
    bool _glockSlideNeedsToBeOut = false;
    bool _needsShotgunFirstShellAdded = false;
    bool _needsShotgunSecondShellAdded = false;
    float m_mouseSensitivity = 0.002f;
    float m_walkSpeed = 5.0f; 
    float m_cameraZoom = 1.0f; 
    float m_accuracyModifer = 0;
    int32_t m_characterModelAnimatedGameObjectIndex = 0;
    int32_t m_viewWeaponAnimatedGameObjectIndex = 0;
    int32_t m_viewportIndex = 0;
    Camera m_camera;
    InputType _inputType = KEYBOARD_AND_MOUSE;
    PlayerControls _controls;
    ShellEjectionState m_shellEjectionState;
    WeaponAction _weaponAction = DRAW_BEGIN;
    std::vector<WeaponState> m_weaponStates;
    std::vector<AmmoState> m_ammoStates;

    float m_waterImpactVelocity = 0;


    float _muzzleFlashRotation = 0;
    glm::vec2 _weaponSwayFactor = glm::vec2(0);
    glm::vec3 _weaponSwayTargetPos = glm::vec3(0);
    float _muzzleFlashCounter = 0;

    uint32_t _interactFlags;
    uint32_t _bulletFlags;
    int m_playerIndex;

    public:
        float _muzzleFlashTimer = 0;

        glm::vec2 m_headBob = glm::vec2(0, 0);
        glm::vec2 m_breatheBob = glm::vec2(0, 0);
        Transform m_weaponSwayTransform;

        glm::mat4 m_weaponSwayMatrix = glm::mat4(1);
        int m_mouseIndex = -1;
        int m_keyboardIndex = -1;
};