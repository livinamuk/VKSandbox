#pragma once
#include <string>
#include "../API/OpenGL/Types/GL_mesh2D.hpp"
#include "FontSpriteSheet.h"

namespace TextBlitter {
    void AddFont(const FontSpriteSheet& font);
    MeshData2D BlitText(const std::string& text, const std::string& fontName, int originX, int originY, glm::ivec2 viewportSize, Alignment alignment, float scale, uint32_t baseVertex);
    FontSpriteSheet* GetFontSpriteSheet(const std::string& name);
    glm::ivec2 GetBlitTextSize(const std::string& text, const std::string& fontName, float scale);
}