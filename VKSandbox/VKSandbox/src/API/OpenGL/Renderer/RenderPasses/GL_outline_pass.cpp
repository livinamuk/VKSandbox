#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_BackEnd.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    std::vector<glm::ivec2> GenerateOutlineOffsets(int lineThickness = 1) {
        std::vector<glm::ivec2> offsets;
        for (int y = -lineThickness; y <= lineThickness; y++) {
            for (int x = -lineThickness; x <= lineThickness; x++) {
                // Only include the outer perimeter of the square ring
                if (abs(x) == lineThickness || abs(y) == lineThickness) {
                    offsets.emplace_back(x, y);
                }
            }
        }
        return offsets;
    }

    void OutlinePass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* outlineFBO = GetFrameBuffer("Outline");
        OpenGLShader* maskShader = GetShader("OutlineMask");
        OpenGLShader* outlineShader = GetShader("Outline");
        OpenGLShader* compositeShader = GetShader("OutlineComposite");

        // Compute offsets given the outline width
        const int outlineWidth = 2;
        static std::vector<glm::ivec2> offsets = GenerateOutlineOffsets(outlineWidth);

        //Setup
        outlineFBO->BindDepthAttachmentFrom(*gBuffer);
        outlineFBO->Bind();
        outlineFBO->ClearAttachmentI("Mask", 0);
        outlineFBO->ClearAttachmentI("Result", 0);
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glDisable(GL_DEPTH_TEST);

        // For each viewport
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            // Render the mask (by drawing all the mesh into it)
            glDrawBuffer(outlineFBO->GetColorAttachmentSlotByName("Mask"));
            glDisable(GL_BLEND);
            maskShader->Use();
            maskShader->SetInt("u_viewportIndex", i);
            for (const RenderItem& renderItem : RenderDataManager::GetOutlineRenderItems()) {
                maskShader->SetMat4("u_modelMatrix", renderItem.modelMatrix);
                Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), mesh->baseVertex);
            }

            // Render the outline (by drawing an instanced quad offset many times)
            outlineShader->Use();
            outlineShader->SetIVec2Array("u_offsets", offsets);
            outlineShader->SetInt("u_offsetCount", offsets.size());
            outlineShader->SetInt("u_viewportIndex", i);
            int instanceCount = offsets.size();
            Mesh* mesh = AssetManager::GetMeshByModelNameMeshIndex("Quad", 0);
            glEnable(GL_BLEND);
            glBlendEquation(GL_MAX);
            glBlendFunc(GL_ONE, GL_ONE);
            glDrawBuffer(outlineFBO->GetColorAttachmentSlotByName("Result"));
            glBindImageTexture(0, outlineFBO->GetColorAttachmentHandleByName("Mask"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
            glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), instanceCount, mesh->baseVertex);
        }

        // Composite the outline
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindImageTexture(1, outlineFBO->GetColorAttachmentHandleByName("Mask"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        glBindImageTexture(2, outlineFBO->GetColorAttachmentHandleByName("Result"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        compositeShader->Use();
        glDispatchCompute(gBuffer->GetWidth() / 16, gBuffer->GetHeight() / 16, 1);

        // Clean Up
        glBlendEquation(GL_FUNC_ADD);
    }
}