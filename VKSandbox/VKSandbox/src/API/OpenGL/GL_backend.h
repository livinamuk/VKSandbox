#pragma once
#include "HellEnums.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "Types/GL_texture.h"
#include "../Renderer/Types/Texture.h"

namespace OpenGLBackEnd {
    // Core
    void Init();
    void BeginFrame();

    // Mouse picking
    void SetMousePickHandles(GLuint frameBufferHandle, GLuint textureHandle);
    void UpdateMousePicking(GLint x, GLint y);
    uint16_t GetMousePickR();
    uint16_t GetMousePickG();
    uint16_t GetMousePickB();
    uint16_t GetMousePickA();

    // Textures
    void UpdateTextureBaking();
    void AllocateTextureMemory(Texture& texture);
    void ImmediateBake(QueuedTextureBake& queuedTextureBake);
    void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake);
    void CleanUpBakingPBOs();
    const std::vector<GLuint64>& GetBindlessTextureIDs();

    // Buffers
    void UploadVertexData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    GLuint GetVertexDataVAO();
    GLuint GetVertexDataVBO();
    GLuint GetVertexDataEBO();
}