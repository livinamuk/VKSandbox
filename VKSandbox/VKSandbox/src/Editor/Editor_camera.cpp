#pragma once
#include "Editor.h"
#include "HellDefines.h"
#include "Gizmo.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include <unordered_map>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

namespace Editor {
    float g_zoomSpeed = 0.4f;
    float g_panSpeed = 0.0075f;

    float g_cameraInterolationDuration = 0.2f;
    float g_interpolationCounter[4];
    glm::vec3 g_cameraPositionCurrent[4];
    glm::vec3 g_cameraPositionTarget[4];
    glm::vec3 g_cameraUpCurrent[4];
    glm::vec3 g_cameraUpTarget[4];
    glm::vec3 g_orbitPosition[4];
    CameraView g_cameraView[4];
    CameraView g_cameraViewPrevious[4];
    glm::mat4 g_viewMatrices[4];

    std::unordered_map<CameraView, glm::vec3> viewDirections = {
            { CameraView::FRONT,  glm::vec3(0,  0,  1) },
            { CameraView::BACK,   glm::vec3(0,  0, -1) },
            { CameraView::RIGHT,  glm::vec3(1,  0,  0) },
            { CameraView::LEFT,   glm::vec3(-1,  0,  0) },
            { CameraView::TOP,    glm::vec3(0,  1,  0) },
            { CameraView::BOTTOM, glm::vec3(0, -1,  0) }
    };

    std::unordered_map<CameraView, glm::vec3> g_cameraViewForwardVectors = {
        { CameraView::FRONT,  glm::vec3( 0, 0,-1) },
        { CameraView::BACK,   glm::vec3( 0, 0, 1) },
        { CameraView::RIGHT,  glm::vec3(-1, 0, 0) },
        { CameraView::LEFT,   glm::vec3( 1, 0, 0) },
        { CameraView::TOP,    glm::vec3( 0,-1, 0) },
        { CameraView::BOTTOM, glm::vec3( 0, 1, 0) }
    };

    std::unordered_map<CameraView, glm::vec3> viewRotations = {
        { CameraView::FRONT,   glm::vec3(0.0f,             0.0f,            0.0f) },
        { CameraView::BACK,    glm::vec3(0.0f,             HELL_PI,         0.0f) },
        { CameraView::RIGHT,   glm::vec3(0.0f,             HELL_PI / 2.0f,  0.0f) },
        { CameraView::LEFT,    glm::vec3(0.0f,            -HELL_PI / 2.0f,  0.0f) },
        { CameraView::TOP,     glm::vec3(-HELL_PI / 2.0f, -HELL_PI / 2.0f,  0.0f) },
        { CameraView::BOTTOM,  glm::vec3(HELL_PI / 2.0f, -HELL_PI / 2.0f,  0.0f) }
    };

    std::unordered_map<CameraView, glm::vec3> upVectors = {
        { CameraView::FRONT,  glm::vec3(0,  1,  0) },
        { CameraView::BACK,   glm::vec3(0,  1,  0) },
        { CameraView::RIGHT,  glm::vec3(0,  1,  0) },
        { CameraView::LEFT,   glm::vec3(0,  1,  0) },
        { CameraView::TOP,    glm::vec3(0,  0, -1) },
        { CameraView::BOTTOM, glm::vec3(0,  0, -1) }
    };

#define ORTHO_CAMERA_DISTANCE_FROM_ORIGIN 250.0f // Rethink this

    void UpdateCamera() {
        Viewport* viewport = ViewportManager::GetViewportByIndex(Editor::GetActiveViewportIndex());
        //Camera* camera = Editor::GetCameraByIndex(Editor::GetActiveViewportIndex());

        // Reset
        if (Input::KeyPressed(HELL_KEY_R)) {
            ResetViewports();
        }

        // Pan
        if (Input::MiddleMouseDown() && Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
            //glm::vec3 offset = glm::vec3(0);
            //offset += (-camera->GetRight() * g_panSpeed * Input::GetMouseOffsetX());
            //offset += (camera->GetUp() * g_panSpeed * Input::GetMouseOffsetY());
            //camera->SetPosition(camera->GetPosition() + offset);


            int i = Editor::GetActiveViewportIndex();
            glm::vec3 target = g_orbitPosition[i];
            glm::vec3 cameraPosition = g_cameraPositionCurrent[i];
            glm::vec3 up = g_cameraUpCurrent[i]; 
            glm::vec3 forward = glm::normalize(target - cameraPosition);
            glm::vec3 right = glm::normalize(glm::cross(forward, up));

            float speed = g_panSpeed * 0.25f * viewport->GetOrthoSize();

            g_cameraPositionCurrent[i] -= (right * speed * Input::GetMouseOffsetX());
            g_cameraPositionCurrent[i] += (up * speed * Input::GetMouseOffsetY());

            g_orbitPosition[i] = g_cameraPositionCurrent[i] + forward * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);

        }

        // Orbit
        // TODO: Implement proper orbit. You just have mouselook atm!!!
        if (Input::MiddleMouseDown() && !Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW) || Input::KeyPressed(HELL_KEY_O)) {
   
            int i = Editor::GetActiveViewportIndex();

            glm::vec3 up = g_cameraUpCurrent[i];
            glm::vec3 currentForward = glm::normalize(g_cameraPositionCurrent[i] - g_orbitPosition[i]);
            glm::vec3 currentRight = glm::normalize(glm::cross(currentForward, up));

            float orbitSpeed = 0.5f;

            //g_orbitPosition[i] = g_cameraPositionCurrent[i] - currentForward * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);
            g_cameraPositionCurrent[i] += currentRight * orbitSpeed * Input::GetMouseOffsetX();
            g_cameraPositionCurrent[i] += up * orbitSpeed * Input::GetMouseOffsetY();

            glm::vec3 newForward = glm::normalize(g_cameraPositionCurrent[i] - g_orbitPosition[i]);
            g_cameraPositionCurrent[i] = g_orbitPosition[i] + newForward * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);

            g_cameraPositionTarget[i] = g_cameraPositionCurrent[i];
            g_cameraUpCurrent[i] = g_cameraUpCurrent[i];
            g_cameraUpTarget[i] = g_cameraUpCurrent[i];
            g_interpolationCounter[i] = 0;

            g_cameraView[i] = CameraView::ORTHO;
            g_cameraViewPrevious[i] = CameraView::ORTHO;
        }
        // Zoom
        float zoomSpeed = g_zoomSpeed;
        if (Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
            zoomSpeed *= 0.5f;
        }
        else if (Input::KeyDown(HELL_KEY_LEFT_CONTROL_GLFW)) {
            zoomSpeed *= 2.5f;
        }
        zoomSpeed *= viewport->GetOrthoSize();

        // Iterate over all viewports, check any for hover
        for (int i = 0; i < 4; i++) {
            Viewport* queryViewport = ViewportManager::GetViewportByIndex(i);
            if (queryViewport->IsHovered()) {
                if (queryViewport->IsOrthographic()) {
                    if (Input::MouseWheelUp()) {
                        queryViewport->SetOrthoSize(queryViewport->GetOrthoSize() - (1 * zoomSpeed));
                    }
                    if (Input::MouseWheelDown()) {
                        queryViewport->SetOrthoSize(queryViewport->GetOrthoSize() + (1 * zoomSpeed));
                    }
                }
                else {
                    //Camera* camera = GetCameraByIndex(i);
                    //camera->SetPosition(camera->GetPosition() + (camera->GetForward() * zoomSpeed * (float)Input::GetMouseWheelValue()));
                }
            }
        }
    }

    glm::mat4 GetViewportViewMatrix(int32_t viewportIndex) {
        return g_viewMatrices[viewportIndex];
    }



    // refactor me, you stole this fromt he old code
    CameraView GetCameraViewByIndex(uint32_t index) {
        if (index >= 0 && index < 4) {
            return g_cameraView[index];
        }
        else {
            std::cout << "Editor::GetViewportTypeByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
            return CameraView::PERSPECTIVE;
        }
    }



    void UpdateCameraInterpolation(float deltaTime) {

        static bool runOnce = true;
        if (runOnce) {

            g_cameraView[0] = CameraView::TOP;
            g_cameraView[1] = CameraView::FRONT;
            g_cameraView[2] = CameraView::LEFT;
            g_cameraView[3] = CameraView::RIGHT;

            g_cameraViewPrevious[0] = CameraView::TOP;
            g_cameraViewPrevious[1] = CameraView::FRONT;
            g_cameraViewPrevious[2] = CameraView::LEFT;
            g_cameraViewPrevious[3] = CameraView::RIGHT;

            for (int i = 0; i < 4; i++) {
                g_orbitPosition[i] = Gizmo::GetPosition();
                g_cameraPositionCurrent[i] = g_orbitPosition[i] + viewDirections[g_cameraView[i]] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);
                g_cameraPositionTarget[i] = g_cameraPositionCurrent[i];
                g_cameraUpCurrent[i] = upVectors[g_cameraView[i]];
                g_cameraUpTarget[i] = upVectors[g_cameraView[i]];
                g_interpolationCounter[i] = 0;
            }
            runOnce = false;
        }


        for (int i = 0; i < 4; i++) {

            // Interpolation
            if (g_interpolationCounter[i] > 0) {

                // Decrement timer
                g_interpolationCounter[i] -= deltaTime;

                // Convert countdown to interpolation factor
                float t = 1.0f - (g_interpolationCounter[i] / g_cameraInterolationDuration);


                if (g_cameraView[i] == CameraView::ORTHO) {


                    if (g_interpolationCounter[i] < 0.0f) {
                        //g_cameraPositionCurrent[i] = g_cameraPositionTarget[i];
                        g_interpolationCounter[i] = 0.0f;
                        g_cameraViewPrevious[i] = g_cameraView[i];
                    }
                }
                else {

                    // Update position
                    glm::vec3 currentForward = glm::normalize(g_cameraPositionCurrent[i] - g_orbitPosition[i]);
                    glm::vec3 targetForward = glm::normalize(g_cameraPositionTarget[i] - g_orbitPosition[i]);
                    glm::quat rotationQuat = glm::rotation(currentForward, targetForward);
                    if (glm::dot(glm::quat(1, 0, 0, 0), rotationQuat) < 0.0f) {
                        rotationQuat = -rotationQuat; // Ensure shortest path in SLERP
                    }
                    glm::quat interpolatedQuat = glm::slerp(glm::quat(1, 0, 0, 0), rotationQuat, t);
                    glm::vec3 newForward = interpolatedQuat * currentForward;
                    float orbitRadius = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
                    g_cameraPositionCurrent[i] = g_orbitPosition[i] + newForward * orbitRadius;

                    // Interpolate camera current up vector to camera target up vector
                    glm::vec3 currentUp = g_cameraUpCurrent[i];
                    glm::vec3 targetUp = g_cameraUpTarget[i];
                    glm::vec3 interpolatedUp = glm::normalize(glm::mix(currentUp, targetUp, t));
                    g_cameraUpCurrent[i] = glm::normalize(glm::mix(currentUp, targetUp, t));

                    if (g_interpolationCounter[i] < 0.0f) {
                        g_cameraPositionCurrent[i] = g_cameraPositionTarget[i];
                        g_interpolationCounter[i] = 0.0f;
                        g_cameraViewPrevious[i] = g_cameraView[i];
                    }
                }
            }

        }

        // View matrices
        for (int i = 0; i < 4; i++) {
            glm::vec3 target = g_orbitPosition[i];
            glm::vec3 cameraPosition = g_cameraPositionCurrent[i];
            glm::vec3 up = g_cameraUpCurrent[i];

            // If ortho use regular lookat
            if (g_cameraView[i] == CameraView::ORTHO) {
                glm::vec3 forward = glm::normalize(target - cameraPosition);
                g_viewMatrices[i] = glm::lookAt(cameraPosition, cameraPosition + forward, up);
            }
            // If the current view is the same as the target, just face camera down its forward vector
            else if (g_cameraViewPrevious[i] == g_cameraView[i]) {
                glm::vec3 trueForward = g_cameraViewForwardVectors[g_cameraView[i]];
                target = g_orbitPosition[i] + trueForward;
                g_viewMatrices[i] = glm::lookAt(cameraPosition, cameraPosition + trueForward, up);
            }
            // Otherwise look at the target as it rotates
            else {
                glm::vec3 forward = glm::normalize(target - cameraPosition);
                g_viewMatrices[i] = glm::lookAt(cameraPosition, cameraPosition + forward, up);
            }
        }
    }




    void SetViewportView(uint32_t viewportIndex, glm::vec3 viewportOrigin, CameraView targetView) {
        g_interpolationCounter[viewportIndex] = g_cameraInterolationDuration;
        g_cameraUpTarget[viewportIndex] = upVectors[targetView];
        g_cameraViewPrevious[viewportIndex] = g_cameraView[viewportIndex];
        g_cameraView[viewportIndex] = targetView;
        g_orbitPosition[viewportIndex] = viewportOrigin;
        g_cameraPositionTarget[viewportIndex] = viewportOrigin + viewDirections[g_cameraView[viewportIndex]] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);;

        //SetCameraView(viewportIndex, targetView);
        SetViewportOrthographicState(viewportIndex, true);
        ViewportManager::UpdateViewports();
    }
}