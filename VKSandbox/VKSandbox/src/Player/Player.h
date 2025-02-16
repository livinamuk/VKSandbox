#pragma once
#include "HellTypes.h"
#include "Camera/Camera.h"
#include "Physics/Physics.h"
#include "Types/AnimatedGameObject.h"
#include "Types/SpriteSheetObject.h"
#include "Weapon/WeaponManager.h"

struct Player {
    void Init(glm::vec3 position, glm::vec3 rotation, int32_t viewportIndex);
    void Update(float deltaTime);
    void Respawn();
    void EnableControl();
    void DisableControl();
    void CreateCharacterController(glm::vec3 position);
    void MoveCharacterController(glm::vec3 displacement);
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

    void UpdateCamera(float deltaTime);
    void UpdateViewWeapon(float deltaTime);
    void UpdateMovement(float deltaTime);
    void UpdateUI();
    void UpdateSpriteSheets(float deltaTime);
    void UpdateHeadBob(float deltaTime);
    void UpdateBreatheBob(float deltaTime);
    void UpdateAudio();

    // Weapon shit
    int GetCurrentWeaponMagAmmo();
    int GetCurrentWeaponTotalAmmo();
    bool CanFire();
    bool CanReload();
    bool CanEnterADS();
    bool InADS(); 
    bool CanMelee();
    bool IsShellInShotgunChamber();
    void UpdateWeaponLogic();
    void GiveDefaultLoadout();
    void GiveWeapon(const std::string& name);
    void GiveAmmo(const std::string& name, int amount);
    void SwitchWeapon(const std::string& name, WeaponAction weaponAction);
    void GiveRedDotToWeapon(const std::string& name);
    void GiveSilencerToWeapon(const std::string& name);
    void DropWeapons();
    void UpdateWeaponSway(float deltaTime);
    WeaponAction& GetWeaponAction();
    WeaponInfo* GetCurrentWeaponInfo();
    WeaponState* GetWeaponStateByName(const std::string &name);
    WeaponState* GetCurrentWeaponState();
    AmmoState* GetAmmoStateByName(const std::string& name);
    AmmoState* GetCurrentAmmoState();
    AmmoInfo* GetCurrentAmmoInfo();

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
    bool IsGrounded();
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

    void DisplayInfoText(const std::string& text);
    std::string m_infoText = "";
    float m_infoTextTimer = 0;

    // Dev keys
    bool PressedFullscreen();
    bool PressedOne();
    bool PressedTwo();
    bool PressedThree();
    bool PressedFour();

private:
    glm::vec3 m_position = glm::vec3(0.0f);

    // Headbob 
    float m_bobOffsetX = 0.0f;
    float m_bobOffsetY = 0.0f;
    float m_headBobTime = 0.0f;
    float m_breatheBobTime = 0.0f;
    glm::vec3 m_headBob = glm::vec3(0.0f);
    glm::vec3 m_breatheBob = glm::vec3(0.0f);

    // Audio
    bool m_footstepPlayed = false;

    // States
    bool m_moving = false;
    bool m_crouching = false;
    bool m_grounded = true;
    bool m_groundedLastFrame;

    // Speed
    float m_walkSpeed = 5.0f;
    float m_currentSpeed = 0.0f;
    float m_walkingSpeed = 4.85f;
    float m_crouchingSpeed = 2.325f;
    float m_swimmingSpeed = 3.25f;
    float m_crouchDownSpeed = 17.5f;

    // Heights
    float m_realViewHeightStanding = 1.65f;  // are these used?
    float m_realViewHeightCrouching = 1.15f; // are these used?
    float m_viewHeightStanding = m_realViewHeightStanding;
    float m_viewHeightCrouching = m_realViewHeightCrouching;
    float m_currentViewHeight = m_viewHeightStanding;

    // Misc
    int m_revolverReloadIterations = 0;
    int m_currentWeaponIndex = 0;
    bool m_controlEnabled = true;
    bool m_awaitingSpawn = true;
    bool m_firedThisFrame = false;
    bool _needsAmmoReloaded = false;
    bool m_revolverNeedsCocking = false;
    bool _glockSlideNeedsToBeOut = false;
    bool _needsShotgunFirstShellAdded = false;
    bool _needsShotgunSecondShellAdded = false;
    float m_mouseSensitivity = 0.002f;
    float m_cameraZoom = 1.0f; 
    float m_accuracyModifer = 0;
    int32_t m_characterModelAnimatedGameObjectIndex = 0;
    int32_t m_viewWeaponAnimatedGameObjectIndex = 0;
    int32_t m_viewportIndex = 0;
    Camera m_camera;
    InputType m_inputType = KEYBOARD_AND_MOUSE;
    PlayerControls m_controls;
    ShellEjectionState m_shellEjectionState;
    WeaponAction m_weaponAction = DRAW_BEGIN;
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

        Transform m_weaponSwayTransform;

        glm::mat4 m_weaponSwayMatrix = glm::mat4(1);
        int m_mouseIndex = -1;
        int m_keyboardIndex = -1;


        float m_weaponAudioFrequency = 1.0f;

        float GetWeaponAudioFrequency();

        void NextWeapon();
        void UpdateMeleeLogic();
        void UpdateGunLogic();
        void UpdateShotgunGunLogic();

        WeaponType GetCurrentWeaponType();
        WeaponAction GetCurrentWeaponAction();

        void FireMelee();
        bool CanFireMelee();
        
        void FireGun();
        bool CanFireGun();


        // Shotgun
        void FireShotgun(); 
        void DryFireShotgun();
        void ReloadShotgun();
        void ToggleAutoShotgun();
        void UpdatePumpAudio();        
        void UpdateShotgunReloadLogic();
        bool CanToggleShotgunAuto();
        bool CanFireShotgun();
        bool CanDryFireShotgun();
        bool CanReloadShotgun();
        bool ShotgunRequiresPump();

        // Casings and Muzzle flash
        void SpawnMuzzleFlash(float speed, float scale);
        void SpawnCasing(AmmoInfo* ammoInfo);


        bool ViewModelAnimationsCompleted();
        bool ViewportIsVisible();

        // Physics 
        void UpdateCharacterController();
        PxShape* GetCharacterControllerShape();
        PxRigidDynamic* GetCharacterControllerActor();

        glm::mat4 GetViewWeaponCameraMatrix();

        glm::mat4 m_viewWeaponCameraMatrix;

        float m_yVelocity = 0;

        float m_weaponSwayX = 0;
        float m_weaponSwayY = 0;

    private:
        SpriteSheetObject m_muzzleFlash;
        PxController* m_characterController = NULL;

        std::vector<SpriteSheetRenderItem> m_spriteSheetRenderItems;

    public:
        const std::vector<SpriteSheetRenderItem>& GetSpriteSheetRenderItems() { return m_spriteSheetRenderItems; }
};