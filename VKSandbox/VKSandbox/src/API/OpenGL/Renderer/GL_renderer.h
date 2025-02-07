#pragma once
#include "Common.h"
#include <string>
#include "../Types/GL_cubemapView.h"
#include "../Types/GL_detachedMesh.hpp"
#include "../Types/GL_frameBuffer.h"
#include "../Types/GL_shader.h"
#include "../Types/GL_ssbo.hpp"

struct OpenGLRasterizerState {
    GLboolean depthTestEnabled;
    GLboolean blendEnable;
    GLboolean cullfaceEnable;
    GLboolean stencilEnable;
    GLboolean depthMask;
    GLfloat pointSize;
    GLenum blendFuncSrcfactor;
    GLenum blendFuncDstfactor;
    GLenum depthFunc;
};

namespace OpenGLRenderer {
    void Init();
    void InitMain();
    void RenderLoadingScreen();
    void RenderGame();

    // Compute passes
    void ComputeSkinningPass();

    // Render passes
    void HeightMapPass();
    void SkyBoxPass();
    void GeometryPass();
    void LightingPass();
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

    void HotloadShaders();

    OpenGLCubemapView* GetCubemapView(const std::string& name);
    OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);
    OpenGLShader* GetShader(const std::string& name);
    OpenGLSSBO* GetSSBO(const std::string& name);

    // Rasterizer State
    void InitRasterizerStates();
    OpenGLRasterizerState* CreateRasterizerState(const std::string& name);
    OpenGLRasterizerState* GetRasterizerState(const std::string& name);
    void SetRasterizerState(const std::string& name);

    // Drawing
    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands);
    void SplitMultiDrawIndirect(OpenGLShader* shader, const std::vector<DrawIndexedIndirectCommand>& commands);
}