#include "../GL_renderer.h"
#include "Editor/Editor.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

#include "Editor/Gizmo.h"

namespace OpenGLRenderer {

    void EditorPass() {
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("SolidColor");

        if (!shader) return;
        if (!Editor::IsEditorOpen()) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "FinalLighting" });

        gBuffer->Bind();
        gBuffer->SetViewport();

        // rewrite this to use your Rasterizer state thing so there is no more funny business!
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {

                OpenGLRenderer::SetViewport(gBuffer, viewport);

                shader->Use();
                shader->SetMat4("projection", viewportData[i].projection);
                shader->SetMat4("view", viewportData[i].view);
                shader->SetBool("useUniformColor", true);

                if (Editor::GetSelectedObjectType() != EditorObjectType::NONE) {
                    for (GizmoRenderItem& renderItem : Gizmo::GetRenderItemsByViewportIndex(i)) {
                        DetachedMesh* mesh = Gizmo::GetDetachedMeshByIndex(renderItem.meshIndex);
                        if (mesh) {
                            OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                            shader->SetMat4("model", renderItem.modelMatrix);
                            shader->SetVec4("uniformColor", renderItem.color);
                            glBindVertexArray(glMesh.GetVAO());
                            glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
                        }
                    }
                }
            }
        }

        // Cleanup
        glDisable(GL_BLEND);
    }
}