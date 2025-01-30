#include "Editor.h"
#include "Util.h"
#include "../Core/Debug.h"
#include "../Config/Config.h"
#include "../Viewport/ViewportManager.h"
#include "../UI/UIBackEnd.h"

namespace Editor {

    void UpdateDebug() {
        if (!IsOpen()) return;

        for (int i = 0; i < 4; i++) {
            const Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            const Camera* camera = Editor::GetCameraByIndex(i);
            const Resolutions& resolutions = Config::GetResolutions();
            int width = resolutions.ui.x * viewport->GetSize().x;
            int height = resolutions.ui.y * viewport->GetSize().y;
            int xLeft = resolutions.ui.x * viewport->GetPosition().x;
            int yTop = resolutions.ui.y * (1.0f - viewport->GetPosition().y - viewport->GetSize().y);

            if (!Debug::IsDebugTextVisible()) {
                std::string text = "";
                //text += "ProjectionMatrix: \n" + Util::Mat4ToString(viewport->GetProjectionMatrix()) + "\n\n";
                //text += "ViewMatrix: \n" + Util::Mat4ToString(camera->GetViewMatrix()) + "\n";
                text += "ViewportMode: " + Util::ViewportModeToString(viewport->GetViewportMode()) + "\n";
                text += "Hover: " + Util::BoolToString(viewport->IsHovered()) + "\n";
                text += "Fuck cunt";
                
                UIBackEnd::BlitText(text, "StandardFont", xLeft, yTop, 2.0f);
            }
        }
    }
}