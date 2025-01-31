#pragma once
#include "Editor.h"
#include "Gizmo.h"
#include "../Input/Input.h"
#include "../Viewport/ViewportManager.h"

namespace Editor {
    float g_zoomSpeed = 0.55f;
    float g_panSpeed = 0.0075f;

    void UpdateCamera() {
        Viewport* viewport = ViewportManager::GetViewportByIndex(Editor::GetActiveViewportIndex());
        Camera* camera = Editor::GetCameraByIndex(Editor::GetActiveViewportIndex());

        // Pan
        if (Input::MiddleMouseDown() && Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
            glm::vec3 offset = glm::vec3(0);
            offset += (-camera->GetRight() * g_panSpeed * Input::GetMouseOffsetX());
            offset += (camera->GetUp() * g_panSpeed * Input::GetMouseOffsetY());
            camera->SetPosition(camera->GetPosition() + offset);
        }

        // Orbit
        // TODO: Implement proper orbit. You just have mouselook atm!!!
        if (Input::MiddleMouseDown() && !Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
            CameraView cameraView = Editor::GetCameraViewByIndex(Editor::GetActiveViewportIndex());
            if (cameraView == CameraView::PERSPECTIVE) {
                camera->Orbit(Input::GetMouseOffsetX(), Input::GetMouseOffsetY());
            }
        }
        // Zoom
        float zoomSpeed = g_zoomSpeed;
        if (Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
            zoomSpeed *= 0.5f;
        }
        else if (Input::KeyDown(HELL_KEY_LEFT_CONTROL_GLFW)) {
            zoomSpeed *= 2.5f;
        }
        // Iterate over all viewports, check any for hover
        for (int i = 0; i < 4; i++) {
            Viewport* queryViewport = ViewportManager::GetViewportByIndex(i);
            if (queryViewport->IsHovered()) {
                if (queryViewport->IsOrthographic()) {
                    if (Input::MouseWheelUp()) {
                        queryViewport->SetOrthoSize(queryViewport->GetOrthoSize() - (1 * g_zoomSpeed));
                    }
                    if (Input::MouseWheelDown()) {
                        queryViewport->SetOrthoSize(queryViewport->GetOrthoSize() + (1 * g_zoomSpeed));
                    }
                }
                else {
                    Camera* camera = GetCameraByIndex(i);
                    camera->SetPosition(camera->GetPosition() + (camera->GetForward() * zoomSpeed * (float)Input::GetMouseWheelValue()));
                }
            }
        }
    }
}