#pragma once
#include "Common.h"
#include <string>
#include "../Types/GL_detachedMesh.hpp"
#include "../Types/GL_shader.h"

namespace OpenGLRenderer {

    void Init();
    void InitMain();
    void RenderLoadingScreen();
    void RenderGame();

    // Debug
    void UpdateDebugLinesMesh();
    void UpdateDebugPointsMesh();
    void DrawPoint(glm::vec3 position, glm::vec3 color);
    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color);
    void DrawAABB(AABB& aabb, glm::vec3 color);
    void DrawAABB(AABB& aabb, glm::vec3 color, glm::mat4 worldTransform);
    inline std::vector<Vertex> g_debugLines;
    inline std::vector<Vertex> g_debugPoints;
    inline OpenGLDetachedMesh g_debugLinesMesh;
    inline OpenGLDetachedMesh g_debugPointsMesh;

    // Shaders
    OpenGLShader* GetShader(const std::string& name);
    void HotloadShaders();
}