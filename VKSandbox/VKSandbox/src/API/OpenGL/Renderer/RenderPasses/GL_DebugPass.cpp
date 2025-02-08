#include "../GL_renderer.h" 
#include "../GL_renderer_util.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Config/Config.h"
#include "Core/Scene.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include <glm/gtx/quaternion.hpp>

#include "Core/Game.h"

namespace OpenGLRenderer {
    void DrawPoint(glm::vec3 position, glm::vec3 color) {
        g_debugPoints.push_back(Vertex(position, color));
    }

    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color) {
        Vertex v0 = Vertex(begin, color);
        Vertex v1 = Vertex(end, color);
        g_debugLines.push_back(v0);
        g_debugLines.push_back(v1);
    }

    void DrawAABB(AABB& aabb, glm::vec3 color) {
        glm::vec3 FrontTopLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z);
        glm::vec3 FrontTopRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z);
        glm::vec3 FrontBottomLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z);
        glm::vec3 FrontBottomRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z);
        glm::vec3 BackTopLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z);
        glm::vec3 BackTopRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z);
        glm::vec3 BackBottomLeft = glm::vec3(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z);
        glm::vec3 BackBottomRight = glm::vec3(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z);
        DrawLine(FrontTopLeft, FrontTopRight, color);
        DrawLine(FrontBottomLeft, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackTopRight, color);
        DrawLine(BackBottomLeft, BackBottomRight, color);
        DrawLine(FrontTopLeft, FrontBottomLeft, color);
        DrawLine(FrontTopRight, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackBottomLeft, color);
        DrawLine(BackTopRight, BackBottomRight, color);
        DrawLine(FrontTopLeft, BackTopLeft, color);
        DrawLine(FrontTopRight, BackTopRight, color);
        DrawLine(FrontBottomLeft, BackBottomLeft, color);
        DrawLine(FrontBottomRight, BackBottomRight, color);
    }

    void DrawAABB(AABB& aabb, glm::vec3 color, glm::mat4 worldTransform) {
        glm::vec3 FrontTopLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 FrontTopRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 FrontBottomLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 FrontBottomRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMax().z, 1.0f);
        glm::vec3 BackTopLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z, 1.0f);
        glm::vec3 BackTopRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMax().y, aabb.GetBoundsMin().z, 1.0f);
        glm::vec3 BackBottomLeft = worldTransform * glm::vec4(aabb.GetBoundsMin().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z, 1.0f);
        glm::vec3 BackBottomRight = worldTransform * glm::vec4(aabb.GetBoundsMax().x, aabb.GetBoundsMin().y, aabb.GetBoundsMin().z, 1.0f);
        DrawLine(FrontTopLeft, FrontTopRight, color);
        DrawLine(FrontBottomLeft, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackTopRight, color);
        DrawLine(BackBottomLeft, BackBottomRight, color);
        DrawLine(FrontTopLeft, FrontBottomLeft, color);
        DrawLine(FrontTopRight, FrontBottomRight, color);
        DrawLine(BackTopLeft, BackBottomLeft, color);
        DrawLine(BackTopRight, BackBottomRight, color);
        DrawLine(FrontTopLeft, BackTopLeft, color);
        DrawLine(FrontTopRight, BackTopRight, color);
        DrawLine(FrontBottomLeft, BackBottomLeft, color);
        DrawLine(FrontBottomRight, BackBottomRight, color);
    }

    void UpdateDebugLinesMesh() {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        vertices.insert(std::end(vertices), std::begin(g_debugLines), std::end(g_debugLines));
        g_debugLines.clear();
        for (int i = 0; i < vertices.size(); i++) {
            indices.push_back(i);
        }
        g_debugLinesMesh.UpdateBuffers(vertices, indices);
    }

    void UpdateDebugPointsMesh() {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        vertices.insert(std::end(vertices), std::begin(g_debugPoints), std::end(g_debugPoints));
        g_debugPoints.clear();
        for (int i = 0; i < vertices.size(); i++) {
            indices.push_back(i);
        }
        g_debugPointsMesh.UpdateBuffers(vertices, indices);
    }




    void DebugPass() {

        OpenGLShader* shader = GetShader("SolidColor");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        if (!shader) return;


        if (Input::KeyPressed(HELL_KEY_5)) {
            Editor::SetSplitX(0.4f);
            Editor::SetSplitY(0.4f);
            ViewportManager::UpdateViewports();
        }
        if (Input::KeyPressed(HELL_KEY_6)) {
            Editor::SetSplitX(1.0f);
            Editor::SetSplitY(1.0f);
            ViewportManager::UpdateViewports();
        }

        if (false) {
            for (RenderItem& renderItem : Scene::GetRenderItems()) {
                Util::UpdateRenderItemAABB(renderItem);
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                DrawAABB(aabb, WHITE);
            }
        }

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        if (false) {
            const Resolutions& resolutions = Config::GetResolutions();

            for (GameObject& gameObject : Scene::GetGameObjects()) {
                Model* model = gameObject.m_model;
                for (auto meshIndex : model->GetMeshIndices()) {
                    Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
                    AABB aabb = AABB(mesh->aabbMin, mesh->aabbMax);
                    glm::vec3 color = glm::vec3(YELLOW);
                    glm::mat4 modelMatrix = gameObject.m_transform.to_mat4();

                    if (model->GetName() == "Cube") {

                        int viewportIndex = Editor::GetHoveredViewportIndex();
                        Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
                        if (!viewport->IsVisible()) continue;

                        glm::vec3 rayOrigin = Editor::GetMouseRayOriginByViewportIndex(viewportIndex);
                        glm::vec3 rayDir = Editor::GetMouseRayDirectionByViewportIndex(viewportIndex);

                        std::vector<Vertex>& vertices = mesh->m_vertices;
                        std::vector<uint32_t>& indices = mesh->m_indices;

                        for (int j = 0; j < indices.size(); j += 3) {
                            Vertex& v0 = vertices[indices[j + 0]];
                            Vertex& v1 = vertices[indices[j + 1]];
                            Vertex& v2 = vertices[indices[j + 2]];
                            glm::vec3 pos0 = modelMatrix * glm::vec4(v0.position, 1.0f);
                            glm::vec3 pos1 = modelMatrix * glm::vec4(v1.position, 1.0f);
                            glm::vec3 pos2 = modelMatrix * glm::vec4(v2.position, 1.0f);

                            float t = 9999;
                            if (Util::RayIntersectsTriangle(rayOrigin, rayDir, pos0, pos1, pos2, t)) {
                                color = glm::vec3(GREEN);
                            }
                        }
                    }
                    DrawAABB(aabb, color, modelMatrix);
                }
            }
        }

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPointSize(8.0f);
        glDisable(GL_DEPTH_TEST);









        std::unordered_map<CameraView, glm::vec3> g_viewDirections = {
 { CameraView::FRONT,  glm::vec3(0,  0,  1) },
 { CameraView::BACK,   glm::vec3(0,  0, -1) },
 { CameraView::RIGHT,  glm::vec3(1,  0,  0) },
 { CameraView::LEFT,   glm::vec3(-1,  0,  0) },
 { CameraView::TOP,    glm::vec3(0,  1,  0) },
 { CameraView::BOTTOM, glm::vec3(0, -1,  0) }
        };


        float ORTHO_CAMERA_DISTANCE_FROM_ORIGIN = 1;
        static glm::vec3 g_orbitTarget = glm::vec3(2, 1, 0);
        static CameraView g_currentView = CameraView::FRONT;

        static glm::vec3 g_camPos = g_orbitTarget + g_viewDirections[g_currentView] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);
        static glm::vec3 g_camPosTarget = g_orbitTarget + g_viewDirections[g_currentView] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);

        static glm::vec3 g_camForward = g_viewDirections[g_currentView];
        static glm::vec3 g_targetForward = g_viewDirections[g_currentView];

        static float timer = 0.0f;
        float timerDuration = 0.5f;

        g_camForward = glm::normalize(g_orbitTarget - g_camPos);
        g_targetForward = glm::normalize(g_orbitTarget - g_camPosTarget);

        float t = 1.0f;
        if (timer > 0 ) {
            timer = glm::max(timer - Game::GetDeltaTime(), 0.0f);
            t = 1.0f - (timer / timerDuration);

            glm::vec3 currentForward = glm::normalize(g_camPos - g_orbitTarget);
            glm::vec3 targetForward = glm::normalize(g_camPosTarget - g_orbitTarget);

            glm::quat rotationQuat = glm::rotation(currentForward, targetForward);

            // Always pick the shortest direction!
            if (glm::dot(glm::quat(1, 0, 0, 0), rotationQuat) < 0.0f) {
                rotationQuat = -rotationQuat;
            }

            glm::quat interpolatedQuat = glm::slerp(glm::quat(1, 0, 0, 0), rotationQuat, t);

            glm::vec3 newForward = interpolatedQuat * currentForward;

            float orbitRadius = ORTHO_CAMERA_DISTANCE_FROM_ORIGIN;
            glm::vec3 g_camInterpolated = g_orbitTarget + newForward * orbitRadius;
            g_camPos = g_camInterpolated;
        }

        if (Input::KeyPressed(HELL_KEY_NUMPAD_1) ||
            Input::KeyPressed(HELL_KEY_NUMPAD_2) ||
            Input::KeyPressed(HELL_KEY_NUMPAD_3) || 
            Input::KeyPressed(HELL_KEY_NUMPAD_4) ||
            Input::KeyPressed(HELL_KEY_NUMPAD_5) ||
            Input::KeyPressed(HELL_KEY_NUMPAD_6)) {

            if (Input::KeyPressed(HELL_KEY_NUMPAD_1)) {
                g_currentView = CameraView::FRONT;
            }
            if (Input::KeyPressed(HELL_KEY_NUMPAD_3)) {
                g_currentView = CameraView::BACK;
            }
            if (Input::KeyPressed(HELL_KEY_NUMPAD_4)) {
                g_currentView = CameraView::LEFT;
            }
            if (Input::KeyPressed(HELL_KEY_NUMPAD_6)) {
                g_currentView = CameraView::RIGHT;
            }
            if (Input::KeyPressed(HELL_KEY_NUMPAD_2)) {
                g_currentView = CameraView::BOTTOM;
            }
            if (Input::KeyPressed(HELL_KEY_NUMPAD_5)) {
                g_currentView = CameraView::TOP;
            }

            timer = timerDuration;
            g_camPosTarget = g_orbitTarget + g_viewDirections[g_currentView] * glm::vec3(ORTHO_CAMERA_DISTANCE_FROM_ORIGIN);

        }
                

        DrawPoint(g_camPosTarget, YELLOW);
        DrawPoint(g_orbitTarget, GREEN);
        DrawPoint(g_camPos, RED);


        DrawLine(g_camPos, g_camPos + (g_camForward * 0.2f), RED);
        DrawLine(g_camPosTarget, g_camPosTarget + (g_targetForward * 0.2f), YELLOW);


        //DrawPoint(oldPos, BLUE);














        EditorMesh& editorMesh = Editor::GetEditorMesh();
        for (int i = 0; i < 8; i++) {
           // DrawPoint(editorMesh.m_corners[i], YELLOW);
        }



        for (int i = 0; i < 8; i++) {
          //  DrawPoint(editorMesh.m_corners[i], YELLOW);
        }



      // const Resolutions& resolutions = Config::GetResolutions();
      //
      //
      // Viewport* viewport = Editor::GetActiveViewport();
      // int viewportIndex = Editor::GetActiveViewportIndex();
      // glm::mat4 projectionView = viewportData[viewportIndex].projectionView;
      //
      // int width = resolutions.gBuffer.x;
      // int height = resolutions.gBuffer.y;
      // int mouseX = Input::GetMouseX();
      // int mouseY = Input::GetMouseY();
      // int threshold = 50;
      //
      // for (int i = 0; i < 8; i++) {
      //
      //     glm::vec3 vertexPosition = editorMesh.m_corners[i];
      //     glm::ivec2 screenPos = Util::WorldToScreenCoords(vertexPosition, projectionView, width, height, true);
      //
      //     if (mouseX < screenPos.x + threshold &&
      //         mouseX > screenPos.x - threshold &&
      //         mouseY < screenPos.y + threshold &&
      //         mouseY > screenPos.y - threshold) {
      //         std::cout << " POINT overlapped \n";
      //
      //         DrawPoint(editorMesh.m_corners[i], BLUE);
      //     }
      //
      // }


//
// glm::ivec2 WorldToScreenCoords(const glm::vec3 & worldPos, const glm::mat4 & viewProjection, int screenWidth, int screenHeight, bool flipY = false);
//
//
// g_hoveredVertexIndex = -1;
// if (g_selectedObjectType == ObjectType::CSG_OBJECT_ADDITIVE_WALL_PLANE) {
//     CSGPlane& csgPlane = Scene::g_csgAdditiveWallPlanes[g_selectedObjectIndex];
//     for (int i = 0; i < 4; i++) {
//         glm::vec3 worldPos = csgPlane.m_veritces[i];
//         glm::ivec2 screenPos = Util::CalculateScreenSpaceCoordinates(worldPos, mvp, PRESENT_WIDTH, PRESENT_HEIGHT, true);
//        
//     }
// }



        shader->Use();
        shader->SetMat4("model", glm::mat4(1));

        UpdateDebugLinesMesh();
        UpdateDebugPointsMesh();

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRendererUtil::SetViewport(gBuffer, viewport);

            glm::mat4 projectionMatrix = viewportData[i].projection;
            glm::mat4 viewMatrix = viewportData[i].view;

            shader->SetMat4("projection", projectionMatrix);
            shader->SetMat4("view", viewMatrix);
            shader->SetBool("useUniformColor", false);
            // Draw lines
            if (g_debugLinesMesh.GetIndexCount() > 0) {
                glBindVertexArray(g_debugLinesMesh.GetVAO());
                glDrawElements(GL_LINES, g_debugLinesMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
            // Draw points
            if (g_debugPointsMesh.GetIndexCount() > 0) {
                glBindVertexArray(g_debugPointsMesh.GetVAO());
                glDrawElements(GL_POINTS, g_debugPointsMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }

            //glEnable(GL_CULL_FACE);
            //
            //editorMesh.RecalculateMesh();
            //shader->SetBool("useUniformColor", true);
            //shader->SetVec4("uniformColor", GREEN);
            //OpenGLDetachedMesh& mesh = editorMesh.m_glMesh;
            //glBindVertexArray(mesh.GetVAO());
            //glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);

        }
    }
}
