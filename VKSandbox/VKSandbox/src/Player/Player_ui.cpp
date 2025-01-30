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
        text += "Cam Forward: " + Util::Vec3ToString(GetCameraForward()) + "\n";
        text += "Cam Rotation: " + Util::Vec3ToString(GetCameraRotation()) + "\n";
        UIBackEnd::BlitText(text, "StandardFont", xLeft, yTop, 2.0f);
    }
}