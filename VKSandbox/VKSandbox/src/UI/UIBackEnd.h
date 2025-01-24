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
        float scale
    );
    void BlitTexture(
        const std::string& textureName,
        glm::ivec2 location,
        Alignment alignment,
        glm::vec4 colorTint = glm::vec4(1,1,1,1),
        glm::ivec2 size = glm::ivec2(-1, -1)
    );
    void EndFrame();
    Mesh2D& GetUIMesh();
    std::vector<UIRenderItem>& GetRenderItems();

}