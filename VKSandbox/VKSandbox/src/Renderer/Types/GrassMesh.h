#pragma once
#include "API/OpenGL/Types/GL_detachedMesh.hpp"

#define GRASS_SIZE 40

struct GrassMesh {

    void Create();

    glm::vec3 m_bladePoints[GRASS_SIZE][GRASS_SIZE];

    OpenGLDetachedMesh glMesh;
};