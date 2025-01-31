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

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        if (false) {
            const Resolutions& resolutions = Config::GetResolutions();

            for (GameObject& gameObject : Scene::g_gameObjects) {
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
        gBuffer->DrawBuffer("Color");

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPointSize(8.0f);
        glDisable(GL_DEPTH_TEST);

        shader->Use();
        shader->SetMat4("model", glm::mat4(1));

        UpdateDebugLinesMesh();
        UpdateDebugPointsMesh();

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRendererUtil::SetViewport(*gBuffer, *viewport);

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
        }
    }
}
