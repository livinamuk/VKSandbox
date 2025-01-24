#pragma once
#include "HellEnums.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "Types/GL_texture.h"
#include "../Types/Texture.h"

namespace OpenGLBackEnd {
    // Core
    void Init();

    // Mouse picking
    void SetMousePickHandles(GLuint frameBufferHandle, GLuint textureHandle);
    void UpdateMousePicking(GLint x, GLint y);
    int GetMousePickR();
    int GetMousePickG();
    int GetMousePickB();
    int GetMousePickA();

    // Textures
    void UpdateTextureBaking();
    void AllocateTextureMemory(Texture& texture);
    void ImmediateBake(QueuedTextureBake& queuedTextureBake);
    void AsyncBakeQueuedTextureBake(QueuedTextureBake& queuedTextureBake);
    void CleanUpBakingPBOs();
}