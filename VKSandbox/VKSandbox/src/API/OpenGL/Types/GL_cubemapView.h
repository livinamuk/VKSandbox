#pragma once
#include "HellTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct CubemapView {
    void CreateCubemap(const std::vector<GLuint>& tex2D);
    GLuint GetHandle() const;

private:
    GLuint m_handle;
};
