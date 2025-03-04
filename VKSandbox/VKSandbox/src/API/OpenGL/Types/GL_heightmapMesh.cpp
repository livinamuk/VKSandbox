#pragma once
#include "GL_heightmapMesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

void OpenGLHeightMapMesh::Create() {
    if (m_vao != 0) {
        CleanUp();
    }
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glBindVertexArray(0);
}

void OpenGLHeightMapMesh::AllocateMemory(int heightMapCount) {
    if (m_vao == 0) {
        Create();
    }

    int chunkSize = HEIGHT_MAP_SIZE / 8;     // For a 256 heightmap, chunkSize = 32
    int chunkWidth = chunkSize + 1;          // 33 vertices per chunk
    int chunkDepth = chunkSize + 1;          // 33 vertices per chunk

    //std::cout << "vertices per heightmap: " << chunkWidth * chunkDepth << "\n";
    //std::cout << "indices per heightmap: " << (chunkWidth - 1) * (chunkDepth - 1) * 6 << "\n";

    // Memory for one chunk:
    int localVertexBufferSize = chunkWidth * chunkDepth * sizeof(Vertex);
    int localIndexBufferSize = (chunkWidth - 1) * (chunkDepth - 1) * 6 * sizeof(uint32_t);

    // Total number of chunks per heightmap:
    int chunksPerHeightMap = 8 * 8;

    // Total memory for all heightmaps:
    int totalVertexBufferSize = localVertexBufferSize * chunksPerHeightMap * heightMapCount;
    int totalIndexBufferSize = localIndexBufferSize * chunksPerHeightMap * heightMapCount;

    // If you need more space, destroy the old buffers and create new ones
    if (m_totalVertexBufferSize < totalVertexBufferSize || m_totalIndexBufferSize < totalIndexBufferSize) {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, totalVertexBufferSize, nullptr, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndexBufferSize, nullptr, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

        m_totalVertexBufferSize = totalVertexBufferSize;
        m_totalIndexBufferSize = totalIndexBufferSize;

        std::cout << "Allocated heightmap memory: " << m_totalVertexBufferSize << " verts " << m_totalIndexBufferSize << " indices\n";
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLHeightMapMesh::CleanUp() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    m_vao = 0;
    m_vbo = 0;
    m_ebo = 0;
}

int OpenGLHeightMapMesh::GetVAO() {
    return m_vao;
}

int OpenGLHeightMapMesh::GetVBO() {
    return m_vbo;
}

int OpenGLHeightMapMesh::GetEBO() {
    return m_ebo;
}
