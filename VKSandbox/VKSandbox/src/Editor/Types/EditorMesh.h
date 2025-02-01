#pragma once
#include "HellTypes.h"
#include "API/OpenGL/Types/GL_detachedMesh.hpp"
#include "Util.h"

struct EditorMesh {

    glm::vec3 m_corners[8];
    OpenGLDetachedMesh m_glMesh;

    enum EditorMeshCorners {
        FTL = 0,
        FTR,
        FBL,
        FBR,
        BTL,
        BTR,
        BBL,
        BBR
    };

    void Init(glm::vec3 offset);
    void RecalculateMesh();
};