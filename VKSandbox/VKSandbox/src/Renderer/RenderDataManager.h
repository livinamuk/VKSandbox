#pragma once
#include "HellTypes.h"
#include <vector>

namespace RenderDataManager {
    void Update();
    const RendererData& GetRendererData();
    const std::vector<ViewportData>& GetViewportData();
    const std::vector<RenderItem>& GetInstanceData();
    const DrawCommandsSet& GetDrawInfoSet();

    void ResetBaseSkinnedVertex();
    void IncrementBaseSkinnedVertex(uint32_t vertexCount);
    uint32_t GetBaseSkinnedVertex();

    inline std::vector<glm::mat4> skinningTransforms;

    int EncodeBaseInstance(int playerIndex, int instanceOffset);
    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset);
}