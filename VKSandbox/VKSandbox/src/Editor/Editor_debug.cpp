#include "Editor.h"
#include "Gizmo.h"
#include "Util.h"
#include "../Core/Debug.h"
#include "../Config/Config.h"
#include "../Input/Input.h"
#include "../Viewport/ViewportManager.h"
#include "../UI/UIBackEnd.h"

namespace Editor {

    void UpdateDebug() {
        if (!IsOpen()) return;

        for (int i = 0; i < 4; i++) {
            const Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) return;

            //const Camera* camera = Editor::GetCameraByIndex(i);
            const Resolutions& resolutions = Config::GetResolutions();
            int width = resolutions.ui.x * viewport->GetSize().x;
            int height = resolutions.ui.y * viewport->GetSize().y;
            int xLeft = resolutions.ui.x * viewport->GetPosition().x;
            int yTop = resolutions.ui.y * (1.0f - viewport->GetPosition().y - viewport->GetSize().y);

            if (!Debug::IsDebugTextVisible() && viewport->GetSize().x > 0.15f && viewport->GetSize().y > 0.2f) {
                std::string text = "";
                //text += "ProjectionMatrix: \n" + Util::Mat4ToString(viewport->GetProjectionMatrix()) + "\n\n";
                //text += "ViewMatrix: \n" + Util::Mat4ToString(camera->GetViewMatrix()) + "\n";
                //text += "ViewportMode: " + Util::ViewportModeToString(viewport->GetViewportMode()) + "\n";
                text += Util::CameraViewToString(Editor::GetCameraViewByIndex(i)) + "\n";
                text += "\n";
            
               // text += "IsActive: " + Util::BoolToString(i == Editor::GetActiveViewportIndex()) + "\n";


                //text += "Mouse ray origin: " + Util::Vec3ToString(Editor::GetMouseRayOriginByViewportIndex(i)) + "\n";
                //text += "Ray dir: " + Util::Vec3ToString(Editor::GetMouseRayDirectionByViewportIndex(i)) + "\n";

               // text += "Cam pos: " + Util::Vec3ToString(camera->GetPosition()) + "\n";
               // text += "Cam rot: " + Util::Vec3ToString(camera->GetEulerRotation()) + "\n";
                //text += "Orbit target: " + Util::Vec3ToString(camera->m_orbitTarget) + "\n";
                //text += "Cam euler: " + Util::Vec3ToString(camera->GetEulerRotation()) + "\n";
                


               // text += "Gizmo Position: " + Util::Vec3ToString(Gizmo::GetPosition()) + "\n";

               // SpaceCoords windowSpaceCoords = viewport->GetWindowSpaceCoords();
               // SpaceCoords gBufferSpaceCoords = viewport->GetGBufferSpaceCoords();
               // 
               // text += "\n";
               // text += "WINDOW SPACE\n";
               // text += " width: " + std::to_string(windowSpaceCoords.width) + "\n";
               // text += " height: " + std::to_string(windowSpaceCoords.height) + "\n";
               // text += " localMouseX: " + std::to_string(windowSpaceCoords.localMouseX) + "\n";
               // text += " localMouseY: " + std::to_string(windowSpaceCoords.localMouseY) + "\n";
               // 
               // text += "\n";
               // text += "GBUFFER SPACE\n";
               // text += " width: " + std::to_string(gBufferSpaceCoords.width) + "\n";
               // text += " height: " + std::to_string(gBufferSpaceCoords.height) + "\n";
               // text += " localMouseX: " + std::to_string(gBufferSpaceCoords.localMouseX) + "\n";
               // text += " localMouseY: " + std::to_string(gBufferSpaceCoords.localMouseY) + "\n";

                UIBackEnd::BlitText(text, "StandardFont", xLeft + 2, yTop + 2, Alignment::TOP_LEFT, 2.0f);
            }
        }
    }
}