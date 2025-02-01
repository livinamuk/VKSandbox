#pragma once
#include "EditorMesh.h"

glm::vec2 CalculateUV(const glm::vec3& vertexPosition, const glm::vec3& faceNormal, const glm::vec3& origin) {
    glm::vec2 uv;
    // Find the dominant axis of the face normal
    glm::vec3 absNormal = glm::abs(faceNormal);
    if (absNormal.x > absNormal.y && absNormal.x > absNormal.z) {
        // Dominant axis is X, project onto YZ plane
        uv.y = (vertexPosition.y - origin.y) / absNormal.x;
        uv.x = (vertexPosition.z - origin.z) / absNormal.x;
        uv.y = 1.0f - uv.y;
        if (faceNormal.x > 0) {
            uv.x = 1.0f - uv.x;
        }
    }
    else if (absNormal.y > absNormal.x && absNormal.y > absNormal.z) {
        // Dominant axis is Y, project onto XZ plane
        uv.y = (vertexPosition.x - origin.x) / absNormal.y;
        uv.x = (vertexPosition.z - origin.z) / absNormal.y;
        uv.y = 1.0f - uv.y;
        if (faceNormal.y < 0) {
            uv.y = 1.0f - uv.y;
        }
    }
    else {
        // Dominant axis is Z, project onto XY plane
        uv.x = (vertexPosition.x - origin.x) / absNormal.z;
        uv.y = (vertexPosition.y - origin.y) / absNormal.z;
        uv.y = 1.0f - uv.y;
        if (faceNormal.z < 0) {
            uv.x = 1.0f - uv.x;
        }
    }
    return uv;
}

void EditorMesh::Init(glm::vec3 offset) {
    m_corners[FTL] = { -0.5f,  0.5f,  0.5f }; 
    m_corners[FTR] = { 0.5f,  0.5f,  0.5f };
    m_corners[FBL] = { -0.5f, -0.5f, 0.5f };
    m_corners[FBR] = { 0.5f, -0.5f,  0.5f };
    m_corners[BTL] = { -0.5f,  0.5f,  -0.5f };
    m_corners[BTR] = { 0.5f,  0.5f,  -0.5f };
    m_corners[BBL] = { -0.5f, -0.5f, -0.5f };
    m_corners[BBR] = { 0.5f, -0.5f,  -0.5f };
    m_corners[FTL] += offset;
    m_corners[FTR] += offset;
    m_corners[FBL] += offset;
    m_corners[FBR] += offset;
    m_corners[BTL] += offset;
    m_corners[BTR] += offset;
    m_corners[BBL] += offset;
    m_corners[BBR] += offset;
}

void EditorMesh::RecalculateMesh() {
    std::vector<Vertex> vertices;

   // Front face
   vertices.push_back(Vertex(m_corners[FBL]));
   vertices.push_back(Vertex(m_corners[FTR]));
   vertices.push_back(Vertex(m_corners[FTL]));
   
   vertices.push_back(Vertex(m_corners[FBR]));
   vertices.push_back(Vertex(m_corners[FTR]));
   vertices.push_back(Vertex(m_corners[FBL]));
   
   // Back face
   vertices.push_back(Vertex(m_corners[BTL]));
   vertices.push_back(Vertex(m_corners[BTR]));
   vertices.push_back(Vertex(m_corners[BBL]));
   
   vertices.push_back(Vertex(m_corners[BBL]));
   vertices.push_back(Vertex(m_corners[BTR]));
   vertices.push_back(Vertex(m_corners[BBR]));
   
   // Left face
   vertices.push_back(Vertex(m_corners[FTR]));
   vertices.push_back(Vertex(m_corners[FBR]));
   vertices.push_back(Vertex(m_corners[BTR]));
   
   vertices.push_back(Vertex(m_corners[BBR]));
   vertices.push_back(Vertex(m_corners[BTR]));
   vertices.push_back(Vertex(m_corners[FBR]));
   
   // Right face
   vertices.push_back(Vertex(m_corners[BTL]));
   vertices.push_back(Vertex(m_corners[BBL]));
   vertices.push_back(Vertex(m_corners[FTL]));
   
   vertices.push_back(Vertex(m_corners[FBL]));
   vertices.push_back(Vertex(m_corners[FTL]));
   vertices.push_back(Vertex(m_corners[BBL]));
   
    // Top face
    vertices.push_back(Vertex(m_corners[FTL]));
    vertices.push_back(Vertex(m_corners[FTR]));
    vertices.push_back(Vertex(m_corners[BTL]));

    vertices.push_back(Vertex(m_corners[FTR]));
    vertices.push_back(Vertex(m_corners[BTR]));
    vertices.push_back(Vertex(m_corners[BTL]));

    // Bottom face
    vertices.push_back(Vertex(m_corners[BBL]));
    vertices.push_back(Vertex(m_corners[FBR]));
    vertices.push_back(Vertex(m_corners[FBL]));

    vertices.push_back(Vertex(m_corners[BBL]));
    vertices.push_back(Vertex(m_corners[BBR]));
    vertices.push_back(Vertex(m_corners[FBR]));

   for (int i = 0; i < vertices.size(); i += 3) {
       glm::vec3 a = vertices[i + 0].position;
       glm::vec3 b = vertices[i + 1].position;
       glm::vec3 c = vertices[i + 2].position;
       glm::vec3 dir = glm::cross(b - a, c - a);
       glm::vec3 normal = glm::normalize(dir);
       vertices[i + 0].normal = normal;
       vertices[i + 1].normal = normal;
       vertices[i + 2].normal = normal;
   }
  

   // Calculate UVS 
   float textureScale = 1.0f;
   float textureOffsetX = 0.0f;
   float textureOffsetY = 0.0f;

   for (Vertex& vertex : vertices) {
       glm::vec3 origin = glm::vec3(0, 0, 0);
       origin = glm::vec3(0);
       vertex.uv = CalculateUV(vertex.position, vertex.normal, origin);
       vertex.uv *= textureScale;
       vertex.uv.x += textureOffsetX;
       vertex.uv.y += textureOffsetY;
   }


   // csgNew.m_aabb = AABB(boundsMin, boundsMax);
   //
   // // Normals and tangents
   // for (int i = 0; i < csgNew.m_vertices.size(); i += 3) {
   //     Util::SetNormalsAndTangentsFromVertices(&csgNew.m_vertices[i], &csgNew.m_vertices[i + 1], &csgNew.m_vertices[i + 2]);
   // }
   //

    // // Shortcuts for UVs
    // glm::vec3& v0 = vert0->position;
    // glm::vec3& v1 = vert1->position;
    // glm::vec3& v2 = vert2->position;
    // glm::vec2& uv0 = vert0->uv;
    // glm::vec2& uv1 = vert1->uv;
    // glm::vec2& uv2 = vert2->uv;
    // // Edges of the triangle : position delta. UV delta
    // glm::vec3 deltaPos1 = v1 - v0;
    // glm::vec3 deltaPos2 = v2 - v0;
    // glm::vec2 deltaUV1 = uv1 - uv0;
    // glm::vec2 deltaUV2 = uv2 - uv0;
    // float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    // glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    // glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
    // glm::vec3 normal = glm::normalize(glm::cross(deltaPos1, deltaPos2));
    // vert0->normal = normal;
    // vert1->normal = normal;
    // vert2->normal = normal;
    // vert0->tangent = tangent;
    // vert1->tangent = tangent;
    // vert2->tangent = tangent;




    std::vector<uint32_t> indices = Util::GenerateSequentialIndices(vertices.size());

    m_glMesh.UpdateBuffers(vertices, indices);
}