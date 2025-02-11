#include "Player.h"
#include "../Config/Config.h"
#include "../Core/Debug.h"
#include "../Editor/Editor.h"
#include "../Util/Util.h"
#include "../UI/UiBackend.h"
#include "../Viewport/ViewportManager.h"

void Player::UpdateUI() {
    if (Editor::IsOpen()) return;

    const Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);
    if (!viewport->IsVisible()) return;

    const Resolutions& resolutions = Config::GetResolutions();
    int width = resolutions.ui.x * viewport->GetSize().x;
    int height = resolutions.ui.y * viewport->GetSize().y;
    int xLeft = resolutions.ui.x * viewport->GetPosition().x;
    int yTop = resolutions.ui.y * (1.0f - viewport->GetPosition().y - viewport->GetSize().y);

    if (!Debug::IsDebugTextVisible()) {
        std::string text = "";
        text += "Cam Pos: " + Util::Vec3ToString(GetCameraPosition()) + "\n";
        //text += "Cam Forward: " + Util::Vec3ToString(GetCameraForward()) + "\n";
        //text += "Cam Euler: " + Util::Vec3ToString(GetCameraRotation()) + "\n";

        if (GetCurrentWeaponInfo()) {
            text += "Weapon: " + GetCurrentWeaponInfo()->name + "\n";
        }


        text += "Weapon Action: " + Util::WeaponActionToString(GetCurrentWeaponAction()) + "\n";


     //  text += "\n";
     //  text += "Weapon matrix\n";
     //  text += Util::Mat4ToString(m_viewWeaponCameraMatrix);
     //
     //  text += "\n";
     //  text += "\n";

        text += "\n";
        text += "Ammo: " + std::to_string(GetCurrentWeaponMagAmmo());
        text += "/";
        text += std::to_string(GetCurrentWeaponTotalAmmo()) + "\n";
        text += "ShellInChamber: " + Util::BoolToString(IsShellInShotgunChamber()) + "\n";
        text += "CanFireShotgun: " + Util::BoolToString(CanFireShotgun()) + "\n";

        WeaponState* weaponState = GetCurrentWeaponState();
        AmmoState* ammoState = GetCurrentAmmoState();
        text += "AwaitingPumpAudio: " + Util::BoolToString(weaponState->shotgunAwaitingPumpAudio) + "\n";


        text += "\n";
        text += "Grounded: " + Util::BoolToString(m_grounded) + "\n";

         
        UIBackEnd::BlitText(text, "StandardFont", xLeft, yTop, 2.0f);
    }
}