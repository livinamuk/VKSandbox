#pragma once
#include "Common.h"
#include <string>
#include "API/OpenGL/Types/GL_cubemapView.h"
#include "API/OpenGL/Types/GL_detachedMesh.hpp"
#include "API/OpenGL/Types/GL_frameBuffer.h"
#include "API/OpenGL/Types/GL_shader.h"
#include "API/OpenGL/Types/GL_ssbo.hpp"
#include "Viewport/Viewport.h"

struct OpenGLRasterizerState {
    GLboolean depthTestEnabled = true;
    GLboolean blendEnable = false;
    GLboolean cullfaceEnable = true;
    GLboolean depthMask = true;
    GLfloat pointSize = 1.0f;
    GLenum blendFuncSrcfactor = GL_SRC_ALPHA;
    GLenum blendFuncDstfactor = GL_ONE_MINUS_SRC_ALPHA;
    GLenum depthFunc = GL_LESS;
};

namespace OpenGLRenderer {
    void Init();
    void InitMain();
    void RenderLoadingScreen();
    void RenderGame();

    // Compute passes
    void ComputeSkinningPass();

    // Render passes
    void DebugPass();
    void EditorPass();
    void EmissivePass();
    void GeometryPass();
    void GrassPass();
    void HairPass();
    void HeightMapPass();
    void LightingPass();
    void SkyBoxPass();
    void SpriteSheetPass();
    void UIPass();

    void RenderHairLayer(const DrawCommands& drawCommands, int peelCount);

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
    void CreateBlurBuffers();
    void DrawQuad();

    OpenGLCubemapView* GetCubemapView(const std::string& name);
    OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);
    OpenGLFrameBuffer* GetBlurBuffer(int viewportIndex, int bufferIndex);
    OpenGLShader* GetShader(const std::string& name);

    // SSBOs
    void CreateSSBO(const std::string& name, float size, GLbitfield flags);
    OpenGLSSBO* GetSSBO(const std::string& name);

    // Misc
    void CreateGrassGeometry();

    // Rasterizer State
    void InitRasterizerStates();
    OpenGLRasterizerState* CreateRasterizerState(const std::string& name);
    OpenGLRasterizerState* GetRasterizerState(const std::string& name);
    void SetRasterizerState(const std::string& name);

    // Drawing
    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands);
    void SplitMultiDrawIndirect(OpenGLShader* shader, const std::vector<DrawIndexedIndirectCommand>& commands);

    // Util
    void SetViewport(OpenGLFrameBuffer* framebuffer, Viewport* viewport);
    void ClearFrameBufferByViewport(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLfloat r, GLfloat g = 0.0f, GLfloat b = 0.0f, GLfloat a = 0.0f);
    void ClearFrameBufferByViewportInt(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLint r, GLint g = 0.0f, GLint b = 0.0f, GLint a = 0.0f);
    void ClearFrameBufferByViewportUInt(OpenGLFrameBuffer* framebuffer, const char* attachmentName, Viewport* viewport, GLuint r, GLuint g = 0.0f, GLuint b = 0.0f, GLuint a = 0.0f);
    void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, GLbitfield mask, GLenum filter);
    void BlitFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const char* srcName, const char* dstName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, const Viewport* viewport);
    void BlitFrameBufferDepth(OpenGLFrameBuffer* srcFrameBuffer, OpenGLFrameBuffer* dstFrameBuffer, BlitRect srcRect, BlitRect dstRect);
    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* srcName, GLbitfield mask, GLenum filter);
    void BlitToDefaultFrameBuffer(OpenGLFrameBuffer* srcFrameBuffer, const char* srcName, BlitRect srcRect, BlitRect dstRect, GLbitfield mask, GLenum filter);
    RenderItem2D CreateRenderItem2D(const std::string& textureName, glm::ivec2 location, glm::ivec2 viewportSize, Alignment alignment, glm::vec3 colorTint = WHITE, glm::ivec2 size = glm::ivec2(-1, -1));
    BlitRect BlitRectFromFrameBufferViewport(OpenGLFrameBuffer* framebuffer, Viewport* viewport);
    GLint CreateQuadVAO();
}