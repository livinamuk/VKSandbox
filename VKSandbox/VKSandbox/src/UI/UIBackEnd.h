#pragma once
#include <string>
#include <vector>
#include "Mesh2D.h"
#include "HellDefines.h"

struct UIRenderItem {
    int baseVertex = 0;
    int baseIndex = 0;
    int indexCount = 0;
    int textureIndex = 0;
    int filter = 0; // 0 for linear, 1 for nearest
};

namespace UIBackEnd {
    void Init();
    void Update();
    void BlitText(
        const std::string& text,
        const std::string& fontName,
        int locationX,
        int locationY,
        Alignment alignment,
        float scale,
        TextureFilter textureFilter = TextureFilter::NEAREST
    );
    void BlitTexture(
        const std::string& textureName,
        glm::ivec2 location,
        Alignment alignment,
        glm::vec4 colorTint = glm::vec4(1,1,1,1),
        glm::ivec2 size = glm::ivec2(-1, -1),
        TextureFilter textureFilter = TextureFilter::NEAREST
    );
    void EndFrame();
    Mesh2D& GetUIMesh();
    std::vector<UIRenderItem>& GetRenderItems();

}