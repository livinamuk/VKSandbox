#include "Player.h"
#include "AssetManagement/AssetManager.h"
#include "Config/Config.h"
#include "Core/Debug.h"
#include "Editor/Editor.h"
#include "Util/Util.h"
#include "UI/TextBlitter.h"
#include "UI/UiBackend.h"
#include "Viewport/ViewportManager.h"

void Player::UpdateUI() {
    if (Editor::IsOpen()) return;

    const Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (!viewport->IsVisible()) return;

    const Resolutions& resolutions = Config::GetResolutions();
    int width = resolutions.ui.x * viewport->GetSize().x;
    int height = resolutions.ui.y * viewport->GetSize().y;
    int xLeft = resolutions.ui.x * viewport->GetPosition().x;
    int xRight = xLeft + width;
    int yTop = resolutions.ui.y * (1.0f - viewport->GetPosition().y - viewport->GetSize().y);
    int yBottom = yTop + height;
    int centerX = xLeft + (width / 2);
    int centerY = yTop + (height / 2);
    int ammoX = xRight - (width * 0.17f);
    int ammoY = yBottom - (height * 0.145f) - TextBlitter::GetFontSpriteSheet("AmmoFont")->m_lineHeight;

    // Info text
    int infoTextX = xLeft + (width * 0.1f);
    int infoTextY = ammoY;

    UIBackEnd::BlitText(m_infoText, "StandardFont", infoTextX, infoTextY, Alignment::TOP_LEFT, 2.0f);
    UIBackEnd::BlitTexture("CrosshairDot", glm::ivec2(centerX, centerY), Alignment::CENTERED, WHITE, glm::ivec2(128, 128));

    if (!Debug::IsDebugTextVisible()) {
        std::string text = "";
        text += "Cam Pos: " + Util::Vec3ToString(GetCameraPosition()) + "\n";
        //text += "Cam Euler: " + Util::Vec3ToString(GetCameraRotation()) + "\n";
        text += "Weapon Action: " + Util::WeaponActionToString(GetCurrentWeaponAction()) + "\n";
        
        //text += "CanReloadShotgun: " + Util::BoolToString(CanReloadShotgun()) + "\n";
       // text += "ShellInChamber: " + Util::BoolToString(IsShellInShotgunChamber()) + "\n";
       // text += "CanFireShotgun: " + Util::BoolToString(CanFireShotgun()) + "\n";
       //
       // WeaponState* weaponState = GetCurrentWeaponState();
       // AmmoState* ammoState = GetCurrentAmmoState();
       // text += "AwaitingPumpAudio: " + Util::BoolToString(weaponState->shotgunAwaitingPumpAudio) + "\n";
       // text += "ShotgunInAuto: " + Util::BoolToString(weaponState->shotgunInAutoMode) + "\n";
       //
       //
       // text += "\n";
       // text += "Grounded: " + Util::BoolToString(m_grounded) + "\n";
        //text += "Grounded: " + Util::BoolToString(m_grounded) +  "\n";
        //text += "GroundedLastFrame: " + Util::BoolToString(m_groundedLastFrame) + "\n";
        
        
         
        UIBackEnd::BlitText(text, "StandardFont", xLeft, yTop, Alignment::TOP_LEFT, 2.0f);

        // Ammo
        if (GetCurrentWeaponType() != WeaponType::MELEE) {
            float scale = 1.3f;
            float smallScale = scale * 0.8f;

            int slashPadding = 10;
            std::string clipText = std::to_string(GetCurrentWeaponMagAmmo());
            std::string totalText = std::to_string(GetCurrentWeaponTotalAmmo());

            // Mag ammo color
            if (GetCurrentWeaponMagAmmo() == 0) {
                clipText = "[COL=0.8,0.05,0.05,1]" + clipText;
            }
            else {
                clipText = "[COL=0.16,0.78,0.23,1]" + clipText;
            }

            UIBackEnd::BlitText(clipText, "AmmoFont", ammoX - slashPadding, ammoY, Alignment::TOP_RIGHT, scale, TextureFilter::LINEAR);
            UIBackEnd::BlitText("/", "AmmoFont", ammoX, ammoY, Alignment::CENTERED_HORIZONTAL, scale, TextureFilter::LINEAR);
            UIBackEnd::BlitText(totalText, "AmmoFont", ammoX + slashPadding, ammoY, Alignment::TOP_LEFT, smallScale, TextureFilter::LINEAR);

            // SPAS AUTO
            WeaponState* weaponState = GetCurrentWeaponState();
            WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
            if (weaponInfo->hasAutoSwitch) {
                Texture* texture = AssetManager::GetTextureByName("Weapon_Auto");
                if (GetCurrentWeaponType() == WeaponType::SHOTGUN && texture) {
                    int modifierPadding = 29;
                    int modifierX = TextBlitter::GetBlitTextSize(totalText, "AmmoFont", smallScale).x + modifierPadding;
                    int gridSize = 10;
                    modifierX = (modifierX / 10) * 10;
                    int modifierScaleX = texture->GetWidth(0) * smallScale;
                    int modifierScaleY = texture->GetHeight(0) * smallScale;
                    glm::vec4 unselectedColor = glm::vec4(0.541, 0.51, 0.392, 0.5f);
                    glm::vec4 colorAuto = weaponState->shotgunInAutoMode ? WHITE : unselectedColor;
                    glm::vec4 colorPump = weaponState->shotgunInAutoMode ? unselectedColor : WHITE;
                    float padding = 4;
                    float autoY = ammoY;
                    float pumpY = autoY + ((texture->GetHeight(0) + padding) * smallScale);
                    UIBackEnd::BlitTexture("Weapon_Auto", glm::ivec2(ammoX + modifierX, autoY), Alignment::TOP_LEFT, colorAuto, glm::ivec2(modifierScaleX, modifierScaleY), TextureFilter::LINEAR);
                    UIBackEnd::BlitTexture("Weapon_Pump", glm::ivec2(ammoX + modifierX, pumpY), Alignment::TOP_LEFT, colorPump, glm::ivec2(modifierScaleX, modifierScaleY), TextureFilter::LINEAR);

                    //modifierX += 40;
                    //UIBackEnd::BlitText("AUTO", "StandardFont", ammoX + modifierX, autoY, Alignment::TOP_RIGHT, 1.0, TextureFilter::NEAREST);
                    //UIBackEnd::BlitText("PUMP", "StandardFont", ammoX + modifierX, pumpY, Alignment::TOP_RIGHT, 1.0, TextureFilter::NEAREST);
                }
            }
        }

    }
}