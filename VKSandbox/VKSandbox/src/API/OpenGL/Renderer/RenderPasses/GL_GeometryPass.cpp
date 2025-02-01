#include "../GL_renderer.h" 
#include "../GL_renderer_util.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Viewport/ViewportManager.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"

namespace OpenGLRenderer {

    void GeometryPass() {
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("GBuffer");
        OpenGLShader* editorMeshShader = GetShader("EditorMesh");

        if (!gBuffer) return;
        if (!shader) return;
        if (!editorMeshShader) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition" });









        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        editorMeshShader->Use();

        EditorMesh& editorMesh = Editor::GetEditorMesh();
        editorMesh.RecalculateMesh();

        Material* material = AssetManager::GetDefaultMaterial();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_basecolor)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_normal)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_rma)->GetGLTexture().GetHandle());

        OpenGLDetachedMesh& mesh = editorMesh.m_glMesh;
        glBindVertexArray(mesh.GetVAO());
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRendererUtil::SetViewport(*gBuffer, *viewport);
                editorMeshShader->SetMat4("projection", viewportData[i].projection);
                editorMeshShader->SetMat4("view", viewportData[i].view);
                editorMeshShader->SetMat4("model", glm::mat4(1));
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
        }

        shader->Use();


        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        SetRasterizerState("GeometryPass_NonBlended");
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRendererUtil::SetViewport(*gBuffer, *viewport);
                MultiDrawIndirect(drawInfoSet.geometry.perViewport[i]);
            }
        }

        //gBuffer->DrawBuffers({ "BaseColor" });
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick" });
        
        SetRasterizerState("GeometryPass_Blended");
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRendererUtil::SetViewport(*gBuffer, *viewport);
                MultiDrawIndirect(drawInfoSet.geometryBlended.perViewport[i]);
            }
        }
    }
}

