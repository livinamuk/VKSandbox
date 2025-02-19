#include "Debug.h"
#include "Util.h"
#include "API/OpenGL/Renderer/GL_Renderer.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "UI/UIBackEnd.h"

namespace Debug {
    std::string g_text = "";
    bool g_showDebugText = false;

    void Update() {
        if (!g_showDebugText) return;

        AddText("Editor State: " + Util::EditorStateToString(Editor::GetEditorState()));

        return;

        const Resolutions& resolutions = Config::GetResolutions();
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        int i = 0;
        glm::mat4 projectionMatrix = viewportData[i].projection;
        glm::mat4 viewMatrix = viewportData[i].view;
        glm::mat4 inverseViewMatrix = viewportData[i].inverseView;
        glm::vec3 viewPos = inverseViewMatrix[3];
        glm::vec3 rayOrigin = viewPos;

        int hoveredViewportIndex = Editor::GetHoveredViewportIndex();
        Viewport* viewport = ViewportManager::GetViewportByIndex(hoveredViewportIndex);

        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();
        int gBufferWidth = resolutions.gBuffer.x;
        int gBufferHeight = resolutions.gBuffer.y;
        int viewportWidth = gBufferWidth * viewport->GetSize().x;
        int viewportHeight = gBufferHeight * viewport->GetSize().y;
        float normalizedMouseX = Util::MapRange(mouseX, 0, windowWidth, 0, gBufferWidth);
        float normalizedMouseY = Util::MapRange(mouseY, 0, windowHeight, 0, gBufferHeight);

        float offsetX = viewport->GetPosition().x * gBufferWidth;
        float offsetY = (1 - viewport->GetPosition().y) * gBufferHeight;

        float localX = normalizedMouseX - offsetX;
        float localY = normalizedMouseY - offsetY + viewportHeight;


        float width = viewport->GetSize().x * BackEnd::GetCurrentWindowWidth();
        float height = viewport->GetSize().y * BackEnd::GetCurrentWindowHeight();
        float left = viewport->GetPosition().x * BackEnd::GetCurrentWindowWidth();
        float right = left + width;
        float top = BackEnd::GetCurrentWindowHeight() - (viewport->GetPosition().y * BackEnd::GetCurrentWindowHeight());
        float bottom = top - height;

        float viewportSpaceMouseX = Util::MapRange(mouseX, left, right, 0, viewportWidth);
        float viewportSpaceMouseY = Util::MapRange(mouseY, bottom, top, 0, viewportHeight);

        AddText("");
        AddText("viewportSpaceMouseX: " + std::to_string(viewportSpaceMouseX));
        AddText("viewportSpaceMouseY: " + std::to_string(viewportSpaceMouseY));
        AddText("");
        AddText("WindowWidth: " + std::to_string(windowWidth));
        AddText("WindowHeight: " + std::to_string(windowHeight));
        AddText("gBufferWidth: " + std::to_string(gBufferWidth));
        AddText("gBufferHeight: " + std::to_string(gBufferHeight));
        AddText("viewportWidth: " + std::to_string(viewportWidth));
        AddText("viewportHeight: " + std::to_string(viewportHeight));
        AddText("mouseX: " + std::to_string(mouseX));
        AddText("mouseY: " + std::to_string(mouseY));
        AddText("localX: " + std::to_string(localX));
        AddText("localY: " + std::to_string(localY));
        AddText("normalizedMouseX: " + std::to_string(normalizedMouseX));
        AddText("normalizedMouseY: " + std::to_string(normalizedMouseY));
        AddText("Hovered Viewport Index: " + std::to_string(hoveredViewportIndex));
        AddText("Mouse ray origin: " + Util::Vec3ToString(Editor::GetMouseRayOriginByViewportIndex(hoveredViewportIndex)));
        AddText("Mouse ray direction: " + Util::Vec3ToString(Editor::GetMouseRayDirectionByViewportIndex(hoveredViewportIndex)));
        //AddText("Hovered viewport cam position: " + Util::Vec3ToString(Editor::GetCameraByIndex(hoveredViewportIndex)->GetPosition()));



        //AddText("MousePick: " + std::to_string(BackEnd::GetMousePickR()) + ", " + std::to_string(BackEnd::GetMousePickG()));
        //AddText("Selected Object: " + Editor::EditorObjectTypeToString(Editor::GetSelectedObjectType()));
        //AddText("");

        //for (int i = 0; i < 4; i++) {
        //    const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        //    AddText("ProjectionMatrix: \n" + Util::Mat4ToString(viewportData[i].projection) + "\n");
        //    AddText("ViewMatrix: \n" + Util::Mat4ToString(viewportData[i].view) + "\n");
        //}
        //
        //for (int i = 0; i < 4; i++) {
        //    AddText(std::to_string(i) + ": " + std::to_string(ViewportManager::GetViewportByIndex(i)->IsVisible()));
        //}
    }

    void AddText(const std::string& text) {
        g_text += text + "\n";
    }

    const std::string& GetText() {
        return g_text;
    }

    void EndFrame() {
        g_text = "";
    }

    void ToggleDebugText() {
        g_showDebugText = !g_showDebugText;
    }

    bool IsDebugTextVisible() {
        return g_showDebugText;
    }
}