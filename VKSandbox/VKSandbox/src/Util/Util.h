#pragma once
#include "HellTypes.h"
#include <vector>

namespace Util {
    // Math
    glm::vec3 GetMouseRayDir(glm::mat4 projection, glm::mat4 view, int windowWidth, int windowHeight, int mouseX, int mouseY);
    bool RayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    glm::ivec2 WorldToScreenCoords(const glm::vec3& worldPos, const glm::mat4& viewProjection, int screenWidth, int screenHeight, bool flipY = false);

    // Mesh
    std::vector<Vertex> GenerateSphereVertices(float radius, int segments);
    std::vector<Vertex> GenerateRingVertices(float sphereRadius, float ringThickness, int ringSegments, int thicknessSegments);
    std::vector<Vertex> GenerateConeVertices(float radius, float height, int segments);
    std::vector<Vertex> GenerateCylinderVertices(float radius, float height, int subdivisions);
    std::vector<Vertex> GenerateCubeVertices();
    std::vector<uint32_t> GenerateRingIndices(int segments, int thicknessSegments);
    std::vector<uint32_t> GenerateSphereIndices(int segments); 
    std::vector<uint32_t> GenerateConeIndices(int segments);
    std::vector<uint32_t> GenerateCylinderIndices(int subdivisions);
    std::vector<uint32_t> GenerateCubeIndices();
    std::vector<uint32_t> GenerateSequentialIndices(int vertexCount);
}