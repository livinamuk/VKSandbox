#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/GL_BackEnd.h"
#include "Renderer/RenderDataManager.h"
#include "World/World.h"

namespace OpenGLRenderer {

    std::vector<glm::ivec2> GenerateOutlineOffsets(int pixelThickness = 1) {
        std::vector<glm::ivec2> offsets;
        int gridSize = pixelThickness * 2 + 1;
        for (int y = -pixelThickness; y <= pixelThickness; ++y) {
            for (int x = -pixelThickness; x <= pixelThickness; ++x) {
                if (x == 0 && y == 0)
                    continue;  // Skip the center.
                offsets.emplace_back(x, y);
            }
        }
        return offsets;
    }

    void ComputeScreenSpaceAABB(const glm::vec3& worldAABBMin, const glm::vec3& worldAABBMax, const glm::mat4& pvm, int screenWidth, int screenHeight, glm::ivec2& outMin, glm::ivec2& outMax) {
        std::vector<glm::vec3> corners = {
            {worldAABBMin.x, worldAABBMin.y, worldAABBMin.z},
            {worldAABBMax.x, worldAABBMin.y, worldAABBMin.z},
            {worldAABBMin.x, worldAABBMax.y, worldAABBMin.z},
            {worldAABBMax.x, worldAABBMax.y, worldAABBMin.z},
            {worldAABBMin.x, worldAABBMin.y, worldAABBMax.z},
            {worldAABBMax.x, worldAABBMin.y, worldAABBMax.z},
            {worldAABBMin.x, worldAABBMax.y, worldAABBMax.z},
            {worldAABBMax.x, worldAABBMax.y, worldAABBMax.z}
        };

        outMin = glm::ivec2(std::numeric_limits<int>::max());
        outMax = glm::ivec2(std::numeric_limits<int>::min());

        for (const auto& corner : corners) {
            glm::vec4 clip = pvm * glm::vec4(corner, 1.0f);
            if (clip.w != 0.0f) {
                clip /= clip.w;
            }
            float ndcX = clip.x;
            float ndcY = clip.y;
            float pixelX = (ndcX * 0.5f + 0.5f) * screenWidth;
            float pixelY = (ndcY * 0.5f + 0.5f) * screenHeight;
            outMin.x = std::min(outMin.x, static_cast<int>(std::floor(pixelX)));
            outMin.y = std::min(outMin.y, static_cast<int>(std::floor(pixelY)));
            outMax.x = std::max(outMax.x, static_cast<int>(std::ceil(pixelX)));
            outMax.y = std::max(outMax.y, static_cast<int>(std::ceil(pixelY)));
        }
    }

    void OutlinePass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* outlineFBO = GetFrameBuffer("Outline");
        OpenGLShader* maskShader = GetShader("OutlineMask");
        OpenGLShader* outlineShader = GetShader("Outline");
        OpenGLShader* compositeShader = GetShader("OutlineComposite");
        const ViewportData& viewportData = RenderDataManager::GetViewportData()[0];

        // Compute offsets given the outline width
        int outlineWidth = 3;
        std::vector<glm::ivec2> offsets = GenerateOutlineOffsets(outlineWidth);

        // Hack the mermaid out of the scene
        GameObject* mermaid = nullptr;
        for (GameObject& gameObject : World::GetGameObjects()) {
            if (gameObject.m_model->GetName() == "Mermaid") {
                mermaid = &gameObject;
                break;
            }
        }
        if (!mermaid) return;

        // Rip out her render items
        std::vector<RenderItem> renderItems;
        renderItems.insert(renderItems.begin(), mermaid->GetRenderItems().begin(), mermaid->GetRenderItems().end());
        renderItems.insert(renderItems.begin(), mermaid->GetRenderItemsHairTopLayer().begin(), mermaid->GetRenderItemsHairTopLayer().end());
        renderItems.insert(renderItems.begin(), mermaid->GetRenderItemsHairBottomLayer().begin(), mermaid->GetRenderItemsHairBottomLayer().end());

        // Compute screenspace bounds of aabbs
        glm::ivec2 globalMin(std::numeric_limits<int>::max());
        glm::ivec2 globalMax(std::numeric_limits<int>::min());
        for (RenderItem& renderItem : renderItems) {
            glm::mat4 pvm = viewportData.projectionView;
            glm::ivec2 localMin, localMax;
            ComputeScreenSpaceAABB(renderItem.aabbMin, renderItem.aabbMax, pvm, gBuffer->GetWidth(), gBuffer->GetHeight(), localMin, localMax);
            globalMin.x = std::min(globalMin.x, localMin.x);
            globalMin.y = std::min(globalMin.y, localMin.y);
            globalMax.x = std::max(globalMax.x, localMax.x);
            globalMax.y = std::max(globalMax.y, localMax.y);
        }

        // Clamp the global bounds to the screen dimensions.
        globalMin.x = std::max(globalMin.x - outlineWidth, 0);
        globalMin.y = std::max(globalMin.y - outlineWidth, 0);
        globalMin.x = std::min(globalMin.x + outlineWidth, (int)gBuffer->GetWidth());
        globalMin.y = std::min(globalMin.y + outlineWidth, (int)gBuffer->GetHeight());
        globalMax.x = std::max(globalMax.x - outlineWidth, 0);
        globalMax.y = std::max(globalMax.y - outlineWidth, 0);
        globalMax.x = std::min(globalMax.x + outlineWidth, (int)gBuffer->GetWidth());
        globalMax.y = std::min(globalMax.y + outlineWidth, (int)gBuffer->GetHeight());

        // Compute shader workgroup size
        int localSize = 16;
        int boxWidth = globalMax.x - globalMin.x;
        int boxHeight = globalMax.y - globalMin.y;
        int numGroupsX = (boxWidth + localSize - 1) / localSize;
        int numGroupsY = (boxHeight + localSize - 1) / localSize;
        glm::ivec2 pixelOffset = globalMin;
        glm::ivec2 invocationBounds = glm::ivec2(boxWidth, boxHeight);

        std::cout << "pixelOffset: " << pixelOffset.x << ", " << pixelOffset.y << "\n";
        std::cout << "boxWidth " << boxWidth << "\n";
        std::cout << "boxHeight " << boxHeight << "\n";
        std::cout << "globalMin: " << globalMin.x << ", " << globalMin.y << "\n";
        std::cout << "globalMax: " << globalMax.x << ", " << globalMax.y << "\n\n";

        //Setup
        outlineFBO->BindDepthAttachmentFrom(*gBuffer);
        outlineFBO->Bind();
        outlineFBO->ClearAttachmentI("Mask", 0);
        outlineFBO->ClearAttachmentI("Result", 0);

        // Render the mask
        glViewport(0, 0, gBuffer->GetWidth(), gBuffer->GetHeight());
        glDrawBuffer(outlineFBO->GetColorAttachmentSlotByName("Mask"));
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        glEnable(GL_DEPTH_TEST); 
        glDepthFunc(GL_LEQUAL); 
        glDepthMask(GL_FALSE);

        maskShader->Use();

        for (RenderItem& renderItem : renderItems) {
            maskShader->SetMat4("u_modelMatrix", renderItem.modelMatrix);
            Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), mesh->baseVertex);
        }

        // Render the outline
        outlineShader->Use();
        outlineShader->SetInt("u_offsetCount", offsets.size());
        outlineShader->SetIVec2("u_pixelOffset", pixelOffset);
        outlineShader->SetIVec2("u_invocationBounds", invocationBounds);
        outlineShader->SetIVec2Array("u_offsets", offsets);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindImageTexture(0, outlineFBO->GetColorAttachmentHandleByName("Mask"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        glBindImageTexture(1, outlineFBO->GetColorAttachmentHandleByName("Result"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8);
        glDispatchCompute(numGroupsX, numGroupsY, 1);

        // Composite the outline
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        glBindImageTexture(0, gBuffer->GetColorAttachmentHandleByName("FinalLighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glBindImageTexture(1, outlineFBO->GetColorAttachmentHandleByName("Mask"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        glBindImageTexture(2, outlineFBO->GetColorAttachmentHandleByName("Result"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8);
        compositeShader->Use();
        compositeShader->SetIVec2("u_pixelOffset", pixelOffset);
        compositeShader->SetIVec2("u_invocationBounds", invocationBounds);
        glDispatchCompute(numGroupsX, numGroupsY, 1);
    }
}