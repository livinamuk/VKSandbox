#pragma once
#include "HellTypes.h"
#include "Types/AnimatedGameObject.h"
#include <vector>

namespace RenderDataManager {
    void BeginFrame();
    void Update();

    const RendererData& GetRendererData();
    const std::vector<GPULight>& GetGPULightData();
    const std::vector<RenderItem>& GetInstanceData();
    const std::vector<RenderItem>& GetOutlineRenderItems();
    const std::vector<ViewportData>& GetViewportData();
    const DrawCommandsSet& GetDrawInfoSet();

    void SubmitAnimatedGameObjectForSkinning(AnimatedGameObject* animatedGameObject);
    void ResetBaseSkinnedVertex();
    void IncrementBaseSkinnedVertex(uint32_t vertexCount);
    uint32_t GetBaseSkinnedVertex();
    std::vector<AnimatedGameObject*>& GetAnimatedGameObjectToSkin();

    inline std::vector<glm::mat4> skinningTransforms;

    int EncodeBaseInstance(int playerIndex, int instanceOffset);
    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset);

    // Submissions
    void SubmitForOutlineRendering(const RenderItem& renderItem);
    void SubmitForOutlineRendering(const std::vector<RenderItem>& renderItems);

}