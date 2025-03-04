#pragma once
#include "HellTypes.h"
#include <vector>

struct OpenGLHeightMapMesh {
private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

public:
    void AllocateMemory(int heightMapCount);
    void CleanUp();
    int GetVAO();
    int GetVBO();
    int GetEBO();

private:
    int m_totalVertexBufferSize = 0;
    int m_totalIndexBufferSize = 0;

    void Create();
};