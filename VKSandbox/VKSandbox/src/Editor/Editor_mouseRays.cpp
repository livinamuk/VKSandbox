#include "Editor.h"
#include "Util.h"
#include "Viewport/ViewportManager.h"

namespace Editor {
    glm::vec3 g_mouseRayOrigins[4];
    glm::vec3 g_mouseRayDirections[4];

    void UpdateMouseRays() {
        for (int i = 0; i < 4; i++) {
            //const Camera* camera = GetCameraByIndex(i);
            const Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            const SpaceCoords gBufferSpaceCoords = viewport->GetGBufferSpaceCoords();

            const glm::mat4 projectionMatrix = viewport->GetProjectionMatrix();
            //const glm::mat4 viewMatrix = camera->GetViewMatrix();

            const glm::mat4 viewMatrix = Editor::GetViewportViewMatrix(i);
            const glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;
            const glm::mat4 inverseProjectionViewMatrix = glm::inverse(projectionViewMatrix);
        
            // Orthographic
            if (viewport->IsOrthographic()) {
                float ndcX = (2.0f * gBufferSpaceCoords.localMouseX / gBufferSpaceCoords.width) - 1.0f;
                float ndcY = 1.0f - (2.0f * (gBufferSpaceCoords.localMouseY / gBufferSpaceCoords.height));
                glm::vec4 nearClip(ndcX, ndcY, -1.0f, 1.0f);
                glm::vec4 farClip(ndcX, ndcY, 1.0f, 1.0f);
                glm::vec4 nearWorld = inverseProjectionViewMatrix * nearClip;
                nearWorld /= nearWorld.w;
                glm::vec4 farWorld = inverseProjectionViewMatrix * farClip;
                farWorld /= farWorld.w;
                g_mouseRayOrigins[i] = glm::vec3(nearWorld);
                g_mouseRayDirections[i] = glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld));
            }
            // Perspective
            else {
               // g_mouseRayOrigins[i] = camera->GetPosition();
               // g_mouseRayDirections[i] = Util::GetMouseRayDir(projectionMatrix, viewMatrix, gBufferSpaceCoords.width, gBufferSpaceCoords.height, gBufferSpaceCoords.localMouseX, gBufferSpaceCoords.localMouseY);
            }
        }
    }

    glm::vec3 GetMouseRayOriginByViewportIndex(int32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_mouseRayOrigins[viewportIndex];
        }
        else {
            std::cout << "Editor::GetMouseRayOriginByViewportIndex(int32_t viewportIndex) Failed because " << viewportIndex << " is out of range of size 4\n";
        }
    }

    glm::vec3 GetMouseRayDirectionByViewportIndex(int32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < 4) {
            return g_mouseRayDirections[viewportIndex];
        }
        else {
            std::cout << "Editor::GetMouseRayDirectionByViewportIndex(int32_t viewportIndex) Failed because " << viewportIndex << " is out of range of size 4\n";
        }
    }
}