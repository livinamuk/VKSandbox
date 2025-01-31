#pragma once
#include "Common.h"
#include <string>
#include "../Types/GL_cubemapView.h"
#include "../Types/GL_detachedMesh.hpp"
#include "../Types/GL_frameBuffer.h"
#include "../Types/GL_shader.h"

namespace OpenGLRenderer {
    void Init();
    void InitMain();
    void RenderLoadingScreen();
    void RenderGame();

    // Render passes
    void SkyBoxPass();
    void GeometryPass();
    void DebugPass();
    void HairPass();
    void RenderHairLayer(const DrawCommands& drawCommands, int peelCount);
    void UIPass();
    void EditorPass();

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

    // Framebuffers
    OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);

    // Framebuffers
    CubemapView* GetCubemapView(const std::string& name);

    // Multidraw Indirect
    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands);
}