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

struct EditorOrthographicCamera {
    CameraView m_cameraView = CameraView::FRONT;
    CameraView m_cameraViewPrevious = CameraView::FRONT;
    glm::vec3 m_positionCurrent = glm::vec3(0);
    glm::vec3 m_positionTarget = glm::vec3(0);
    glm::vec3 m_cameraUpCurrent = glm::vec3(0);
    glm::vec3 m_cameraUpTarget = glm::vec3(0);
    glm::vec3 m_orbitPosition = glm::vec3(0);
    glm::mat4 m_viewMatrix = glm::mat4(1);
    float m_interpolationCounter = 0.0f;
};

namespace Editor {
    float g_zoomSpeed = 0.4f;
    float g_panSpeed = 0.0075f;
    float g_cameraInterolationDuration = 0.2f;
    EditorOrthographicCamera g_orthographicCameras[4];

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

    void ResetCameras() {
        g_orthographicCameras[0].m_positionCurrent = glm::vec3(-191.80, 84.36, 117.22);
        g_orthographicCameras[0].m_positionTarget = glm::vec3(-191.80, 84.36, 117.22);
        g_orthographicCameras[0].m_cameraUpCurrent = glm::vec3(0.00, 1.00, 0.00);
        g_orthographicCameras[0].m_cameraUpTarget = glm::vec3(0.00, 1.00, 0.00);
        g_orthographicCameras[0].m_orbitPosition = glm::vec3(19.60, -4.02, 17.26);
        g_orthographicCameras[0].m_interpolationCounter = 0;

        g_orthographicCameras[1].m_positionCurrent = glm::vec3(-230.36, -3.59, 17.26);
        g_orthographicCameras[1].m_positionTarget = glm::vec3(-230.36, -3.59, 17.26);
        g_orthographicCameras[1].m_cameraUpCurrent = glm::vec3(0.00, 1.00, 0.00);
        g_orthographicCameras[1].m_cameraUpTarget = glm::vec3(0.00, 1.00, 0.00);
        g_orthographicCameras[1].m_orbitPosition = glm::vec3(19.64, -3.75, 17.26);
        g_orthographicCameras[1].m_interpolationCounter = 0;

        g_orthographicCameras[2].m_positionCurrent = glm::vec3(19.54, 250.00, 17.25);
        g_orthographicCameras[2].m_positionTarget = glm::vec3(19.54, 250.00, 17.25);
        g_orthographicCameras[2].m_cameraUpCurrent = glm::vec3(0.00, 0.00, -1.00);
        g_orthographicCameras[2].m_cameraUpTarget = glm::vec3(0.00, 0.00, -1.00);
        g_orthographicCameras[2].m_orbitPosition = glm::vec3(19.54, 0.00, 17.25);
        g_orthographicCameras[2].m_interpolationCounter = 0;

        g_orthographicCameras[3].m_positionCurrent = glm::vec3(19.55, -3.59, 267.33);
        g_orthographicCameras[3].m_positionTarget = glm::vec3(19.55, -3.59, 267.33);
        g_orthographicCameras[3].m_cameraUpCurrent = glm::vec3(0.00, 1.00, 0.00);
        g_orthographicCameras[3].m_cameraUpTarget = glm::vec3(0.00, 1.00, 0.00);
        g_orthographicCameras[3].m_orbitPosition = glm::vec3(19.55, -3.75, 17.33);
        g_orthographicCameras[3].m_interpolationCounter = 0;
    }

    void UpdateCamera() {

       //for (int i = 0; i < 4; i++) {
       //    Viewport* viewport = ViewportManager::GetViewportByIndex(i);
       //    glm::vec3 cameraPosition = g_cameraPositionCurrent[i];
       //    std::cout << "g_cameraPositionCurrent[" << i << "] = glm::vec3" << Util::Vec3ToString(g_cameraPositionCurrent[i]) << ";\n";
       //    std::cout << "g_cameraPositionTarget[" << i << "] = glm::vec3" << Util::Vec3ToString(g_cameraPositionTarget[i]) << ";\n";
       //    std::cout << "g_cameraUpCurrent[" << i << "] = glm::vec3" << Util::Vec3ToString(g_cameraUpCurrent[i]) << ";\n";
       //    std::cout << "g_cameraUpTarget[" << i << "] = glm::vec3" << Util::Vec3ToString(g_cameraUpTarget[i]) << ";\n";
       //    std::cout << "g_orbitPosition[" << i << "] = glm::vec3" << Util::Vec3ToString(g_orbitPosition[i]) << ";\n";
       //    std::cout << "\n";
       //}

        Viewport* viewport = ViewportManager::GetViewportByIndex(Editor::GetActiveViewportIndex());

        // Pan
        if (Input::MiddleMouseDown() && Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
            int i = Editor::GetActiveViewportIndex();
            glm::vec3 target = g_orthographicCameras[i].m_orbitPosition;
            glm::vec3 cameraPosition = g_orthographicCameras[i].m_positionCurrent;
            glm::vec3 up = g_orthographicCameras[i].m_cameraUpCurrent;
            glm::vec3 forward = glm::normalize(target - cameraPosition);
            glm::vec3 right = glm::normalize(glm::cross(forward, up));

            float speed = g_panSpeed * 0.25f * viewport->GetOrthoSize();
            g_orthographicCameras[i].m_positionCurrent -= (right * speed * Input::GetMouseOffsetX());
            g_orthographicCameras[i].m_positionCurrent += (up * speed * Input::GetMouseOffsetY());
            g_orthographicCameras[i].m_orbitPosition = g_orthographicCameras[i].m_positionCurrent + forward * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);
        }

        // Orbit
        if (Input::MiddleMouseDown() && !Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
   
            int i = Editor::GetActiveViewportIndex();

            glm::vec3 up = g_orthographicCameras[i].m_cameraUpCurrent;
            glm::vec3 currentForward = glm::normalize(g_orthographicCameras[i].m_positionCurrent - g_orthographicCameras[i].m_orbitPosition);
            glm::vec3 currentRight = glm::normalize(glm::cross(currentForward, up));

            float orbitSpeed = 0.5f;

            g_orthographicCameras[i].m_positionCurrent += currentRight * orbitSpeed * Input::GetMouseOffsetX();
            g_orthographicCameras[i].m_positionCurrent += up * orbitSpeed * Input::GetMouseOffsetY();

            glm::vec3 newForward = glm::normalize(g_orthographicCameras[i].m_positionCurrent - g_orthographicCameras[i].m_orbitPosition);
            g_orthographicCameras[i].m_positionCurrent = g_orthographicCameras[i].m_orbitPosition + newForward * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);

            g_orthographicCameras[i].m_positionTarget = g_orthographicCameras[i].m_positionCurrent;
            g_orthographicCameras[i].m_cameraUpTarget = g_orthographicCameras[i].m_cameraUpCurrent;
            g_orthographicCameras[i].m_interpolationCounter = 0;

            g_orthographicCameras[i].m_cameraView = CameraView::ORTHO;
            g_orthographicCameras[i].m_cameraViewPrevious = CameraView::ORTHO;
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
        return g_orthographicCameras[viewportIndex].m_viewMatrix;
    }



    // refactor me, you stole this front he old code
    CameraView GetCameraViewByIndex(uint32_t index) {
        if (index >= 0 && index < 4) {
            return g_orthographicCameras[index].m_cameraView;
        }
        else {
            std::cout << "Editor::GetViewportTypeByIndex(int index) failed. " << index << " out of range of editor viewport count 4\n";
            return CameraView::PERSPECTIVE;
        }
    }



    void UpdateCameraInterpolation(float deltaTime) {

        //static bool runOnce = true;
        //if (runOnce) {
        //
        //    g_orthographicCameras[0].m_cameraView = CameraView::TOP;
        //    g_orthographicCameras[1].m_cameraView = CameraView::FRONT;
        //    g_orthographicCameras[2].m_cameraView = CameraView::LEFT;
        //    g_orthographicCameras[3].m_cameraView = CameraView::RIGHT;
        //
        //    g_orthographicCameras[0].m_cameraViewPrevious = CameraView::TOP;
        //    g_orthographicCameras[1].m_cameraViewPrevious = CameraView::FRONT;
        //    g_orthographicCameras[2].m_cameraViewPrevious = CameraView::LEFT;
        //    g_orthographicCameras[3].m_cameraViewPrevious = CameraView::RIGHT;
        //
        //    for (int i = 0; i < 4; i++) {
        //        g_orthographicCameras[i].m_orbitPosition = Gizmo::GetPosition();
        //        g_orthographicCameras[i].m_positionCurrent = g_orthographicCameras[i].m_orbitPosition + viewDirections[g_orthographicCameras[i].m_cameraView] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);
        //        g_orthographicCameras[i].m_positionTarget = g_orthographicCameras[i].m_positionCurrent;
        //        g_orthographicCameras[i].m_cameraUpCurrent = upVectors[g_orthographicCameras[i].m_cameraView];
        //        g_orthographicCameras[i].m_cameraUpTarget = upVectors[g_orthographicCameras[i].m_cameraView];
        //        g_orthographicCameras[i].m_interpolationCounter = 0;
        //    }
        //    runOnce = false;
        //}


        for (int i = 0; i < 4; i++) {

            // Interpolation
            if (g_orthographicCameras[i].m_interpolationCounter > 0) {

                // Decrement timer
                g_orthographicCameras[i].m_interpolationCounter -= deltaTime;

                // Convert countdown to interpolation factor
                float t = 1.0f - (g_orthographicCameras[i].m_interpolationCounter / g_cameraInterolationDuration);


                if (g_orthographicCameras[i].m_cameraView == CameraView::ORTHO) {


                    if (g_orthographicCameras[i].m_interpolationCounter < 0.0f) {
                        g_orthographicCameras[i].m_interpolationCounter = 0.0f;
                        g_orthographicCameras[i].m_cameraViewPrevious = g_orthographicCameras[i].m_cameraView;
                    }
                }
                else {

                    // Update position
                    glm::vec3 currentForward = glm::normalize(g_orthographicCameras[i].m_positionCurrent - g_orthographicCameras[i].m_orbitPosition);
                    glm::vec3 targetForward = glm::normalize(g_orthographicCameras[i].m_positionTarget - g_orthographicCameras[i].m_orbitPosition);
                    glm::quat rotationQuat = glm::rotation(currentForward, targetForward);
                    if (glm::dot(glm::quat(1, 0, 0, 0), rotationQuat) < 0.0f) {
                        rotationQuat = -rotationQuat; // Ensure shortest path in SLERP
                    }
                    glm::quat interpolatedQuat = glm::slerp(glm::quat(1, 0, 0, 0), rotationQuat, t);
                    glm::vec3 newForward = interpolatedQuat * currentForward;
                    float orbitRadius = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
                    g_orthographicCameras[i].m_positionCurrent = g_orthographicCameras[i].m_orbitPosition + newForward * orbitRadius;

                    // Interpolate camera current up vector to camera target up vector
                    glm::vec3 currentUp = g_orthographicCameras[i].m_cameraUpCurrent;
                    glm::vec3 targetUp = g_orthographicCameras[i].m_cameraUpTarget;
                    glm::vec3 interpolatedUp = glm::normalize(glm::mix(currentUp, targetUp, t));
                    g_orthographicCameras[i].m_cameraUpCurrent = glm::normalize(glm::mix(currentUp, targetUp, t));

                    if (g_orthographicCameras[i].m_interpolationCounter < 0.0f) {
                        g_orthographicCameras[i].m_positionCurrent = g_orthographicCameras[i].m_positionTarget;
                        g_orthographicCameras[i].m_interpolationCounter = 0.0f;
                        g_orthographicCameras[i].m_cameraViewPrevious = g_orthographicCameras[i].m_cameraView;
                    }
                }
            }

        }

        // View matrices
        for (int i = 0; i < 4; i++) {
            glm::vec3 target = g_orthographicCameras[i].m_orbitPosition;
            glm::vec3 cameraPosition = g_orthographicCameras[i].m_positionCurrent;
            glm::vec3 up = g_orthographicCameras[i].m_cameraUpCurrent;

            // If ortho use regular lookat
            if (g_orthographicCameras[i].m_cameraView == CameraView::ORTHO) {
                glm::vec3 forward = glm::normalize(target - cameraPosition);
                g_orthographicCameras[i].m_viewMatrix = glm::lookAt(cameraPosition, cameraPosition + forward, up);
            }
            // If the current view is the same as the target, just face camera down its forward vector
            else if (g_orthographicCameras[i].m_cameraViewPrevious == g_orthographicCameras[i].m_cameraView) {
                glm::vec3 trueForward = g_cameraViewForwardVectors[g_orthographicCameras[i].m_cameraView];
                target = g_orthographicCameras[i].m_orbitPosition + trueForward;
                g_orthographicCameras[i].m_viewMatrix = glm::lookAt(cameraPosition, cameraPosition + trueForward, up);
            }
            // Otherwise look at the target as it rotates
            else {
                glm::vec3 forward = glm::normalize(target - cameraPosition);
                g_orthographicCameras[i].m_viewMatrix = glm::lookAt(cameraPosition, cameraPosition + forward, up);
            }
        }
    }




    void SetViewportView(uint32_t viewportIndex, glm::vec3 viewportOrigin, CameraView targetView) {
        g_orthographicCameras[viewportIndex].m_interpolationCounter = g_cameraInterolationDuration;
        g_orthographicCameras[viewportIndex].m_cameraUpTarget = upVectors[targetView];
        g_orthographicCameras[viewportIndex].m_cameraViewPrevious = g_orthographicCameras[viewportIndex].m_cameraView;
        g_orthographicCameras[viewportIndex].m_cameraView = targetView;
        g_orthographicCameras[viewportIndex].m_orbitPosition = viewportOrigin;
        g_orthographicCameras[viewportIndex].m_positionTarget = viewportOrigin + viewDirections[g_orthographicCameras[viewportIndex].m_cameraView] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);;

        //SetCameraView(viewportIndex, targetView);
        SetViewportOrthographicState(viewportIndex, true);
        //ViewportManager::UpdateViewports();
    }
}