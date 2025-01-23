﻿#include "TextBlitter.h"
#include "FontSpriteSheet.h"
#include <unordered_map>
#include <iostream>
#include "../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"

namespace TextBlitter {

    std::vector<FontSpriteSheet> g_fontSpriteSheets;
    std::unordered_map<std::string, uint32_t> g_fontIndices;

    glm::vec3 ParseColorTag(const std::string& tag);

    MeshData2D BlitText(const std::string& text, const std::string& fontName, int originX, int originY, glm::ivec2 viewportSize, float scale, uint32_t baseVertex) {
        FontSpriteSheet* spriteSheet = GetFontSpriteSheet(fontName);
        MeshData2D meshData;
        int textureIndex = AssetManager::GetTextureIndexByName(fontName);

        // Bail if spritesheet or font texture not found
        if (!spriteSheet || textureIndex == -1) {
            return meshData;
        } 
        // Otherwise construct the vertex data
        else {
            float cursorX = static_cast<float>(originX);
            float cursorY = static_cast<float>(viewportSize.y - originY); // Top left corner
            float invTextureWidth = 1.0f / static_cast<float>(spriteSheet->m_textureWidth);
            float invTextureHeight = 1.0f / static_cast<float>(spriteSheet->m_textureHeight);
            float halfPixelU = 0.5f * invTextureWidth;
            float halfPixelV = 0.5f * invTextureHeight;
            glm::vec3 color(1.0f); // Default color

            // Reserve space for vertices and indices
            size_t estimatedVertices = text.length() * 4;
            size_t estimatedIndices = text.length() * 6;
            meshData.vertices.reserve(estimatedVertices);
            meshData.indices.reserve(estimatedIndices);

            for (size_t i = 0; i < text.length();) {

                // Handle color tags
                if (text.compare(i, 5, "[COL=") == 0) {
                    size_t end = text.find("]", i);
                    if (end != std::string::npos) {
                        color = ParseColorTag(text.substr(i, end - i + 1));
                        i = end; // Skip the tag
                        continue;
                    }
                }
                char character = text[i];

                // Handle spaces
                if (character == ' ') {
                    int spaceWidth = spriteSheet->m_charDataList[spriteSheet->m_characters.find(' ')].width;
                    cursorX += spaceWidth * scale;
                    i++;
                    continue;
                }
                // Handle newlines
                if (character == '\n') {
                    cursorX = static_cast<float>(originX);
                    cursorY -= (spriteSheet->m_lineHeight + 1) * scale;
                    i++;
                    continue;
                }
                // Process regular characters
                int charIndex = spriteSheet->m_characters.find(character);
                if (charIndex != std::string::npos) {
                    const auto& charData = spriteSheet->m_charDataList[charIndex];

                    // Normalized uvs
                    float u0 = (charData.offsetX + halfPixelU) * invTextureWidth;
                    float v0 = (charData.offsetY + charData.height - halfPixelV) * invTextureHeight;
                    float u1 = (charData.offsetX + charData.width - halfPixelU) * invTextureWidth;
                    float v1 = (charData.offsetY + halfPixelV) * invTextureHeight;

                    // Normalized quad position
                    float x0 = (cursorX / viewportSize.x) * 2.0f - 1.0f;
                    float y0 = (cursorY / viewportSize.y) * 2.0f - 1.0f;
                    float x1 = ((cursorX + charData.width * scale) / viewportSize.x) * 2.0f - 1.0f;
                    float y1 = ((cursorY - charData.height * scale) / viewportSize.y) * 2.0f - 1.0f;

                    // Vertices
                    meshData.vertices.push_back({ {x0, y0}, {u0, v1}, color, textureIndex }); // Bottom left
                    meshData.vertices.push_back({ {x1, y0}, {u1, v1}, color, textureIndex }); // Bottom right
                    meshData.vertices.push_back({ {x1, y1}, {u1, v0}, color, textureIndex }); // Top right
                    meshData.vertices.push_back({ {x0, y1}, {u0, v0}, color, textureIndex }); // Top left

                    // Indices
                    uint32_t vertexOffset = static_cast<uint32_t>(meshData.vertices.size()) - 4;
                    meshData.indices.push_back(baseVertex + vertexOffset + 0);
                    meshData.indices.push_back(baseVertex + vertexOffset + 1);
                    meshData.indices.push_back(baseVertex + vertexOffset + 2);
                    meshData.indices.push_back(baseVertex + vertexOffset + 0);
                    meshData.indices.push_back(baseVertex + vertexOffset + 2);
                    meshData.indices.push_back(baseVertex + vertexOffset + 3);

                    cursorX += charData.width * scale;
                }
                i++;
            }
            return meshData;
        }
    }

    void AddFont(const FontSpriteSheet& spriteSheet) {
        if (g_fontIndices.find(spriteSheet.m_name) != g_fontIndices.end()) {
            std::cout << "Font already exists: " << spriteSheet.m_name << "\n";
            return;
        }
        uint32_t index = g_fontSpriteSheets.size();
        g_fontIndices[spriteSheet.m_name] = index;
        g_fontSpriteSheets.push_back(spriteSheet);
    }

    FontSpriteSheet* GetFontSpriteSheet(const std::string& name) {
        auto it = g_fontIndices.find(name);
        return (it != g_fontIndices.end()) ? &g_fontSpriteSheets[it->second] : nullptr;
    }

    glm::vec3 ParseColorTag(const std::string& tag) {
        glm::vec3 color(1.0f);
        size_t start = tag.find("[COL=") + 5;
        if (start == std::string::npos) {
            return color;
        }
        size_t end = tag.find("]", start);
        if (end == std::string::npos) {
            return color;
        }
        const char* cStr = tag.c_str() + start;
        char* endPtr;
        color.r = std::strtof(cStr, &endPtr);
        if (*endPtr != ',') {
            return color;
        }
        color.g = std::strtof(endPtr + 1, &endPtr);
        if (*endPtr != ',') {
            return color;
        }
        color.b = std::strtof(endPtr + 1, &endPtr);
        return color;
    }
}