#pragma once
#include "GL_heightmapMesh.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

void OpenGLHeightMapMesh::Create(int size) {
    if (m_vao != 0) {
        CleanUp();
    }
    int vertexBufferSize = size * size * sizeof(Vertex);
    int indexBufferSize = (size - 1) * (size - 1) * 6 * sizeof(uint32_t);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, nullptr, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
