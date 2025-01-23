#pragma once
#include <string>
#include <vector>
#include "Mesh2D.h"
#include "HellDefines.h"

struct UIRenderItem {
    int baseVertex;
    int baseIndex;
    int indexCount;
    int textureIndex;
};

namespace UIBackEnd {
    void Init();
    void Update();
    void BlitText(
        const std::string& text,
        const std::string& fontName,
        int locationX,
        int locationY,
        glm::ivec2 viewportSize,
        float scale
    );
    void BlitTexture(
        const std::string& textureName,
        glm::ivec2 location,
        glm::ivec2 viewportSize,
        Alignment alignment,
        glm::vec3 colorTint = WHITE,
        glm::ivec2 size = glm::ivec2(-1, -1)
    );
    void EndFrame();
    Mesh2D& GetUIMesh();
    std::vector<UIRenderItem>& GetRenderItems();

}