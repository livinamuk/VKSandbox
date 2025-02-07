#pragma once
#include "HellTypes.h"

namespace Renderer {
    void InitMain();
    void RenderLoadingScreen();
    void RenderGame();
    void HotloadShaders();

    void DrawPoint(glm::vec3 position, glm::vec3 color);
    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color);
    void DrawAABB(AABB& aabb, glm::vec3 color);
    void DrawAABB(AABB& aabb, glm::vec3 color, glm::mat4 worldTransform);
}