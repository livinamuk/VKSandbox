#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include <vector>
#include <filesystem>

namespace Util {
    // Math
    glm::ivec2 WorldToScreenCoords(const glm::vec3& worldPos, const glm::mat4& viewProjection, int screenWidth, int screenHeight, bool flipY = false);
    glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b);
    glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b);
    int RandomInt(int min, int max);
    float FInterpTo(float current, float target, float deltaTime, float interpSpeed);
    float RandomFloat(float min, float max);
    float MapRange(float inValue, float minInRange, float maxInRange, float minOutRange, float maxOutRange);

    // Raycasting
    glm::vec3 GetMouseRayDir(glm::mat4 projection, glm::mat4 view, int windowWidth, int windowHeight, int mouseX, int mouseY);
    bool RayIntersectsTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);

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

    // String
    std::string BoolToString(bool b);
    std::string Vec3ToString(glm::vec3 v);
    std::string Mat4ToString(glm::mat4 m);
    std::string Lowercase(std::string& str);
    std::string Uppercase(std::string& str);
    std::string ViewportModeToString(const ShadingMode& viewportMode);
    std::string CameraTypeToString(const CameraView& cameraView);
    std::string ViewportResizeStateToString(const ViewportResizeState& viewportResizeState);

    // File
    std::string GetFilename(const std::string& filepath);
    std::string GetFileName(const std::string& filepath);
    std::string RemoveFileExtension(const std::string& filename);
    std::string GetFullPath(const std::filesystem::directory_entry& entry);
    std::string GetFileName(const std::filesystem::directory_entry& entry);
    std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry);
    std::string GetFileExtension(const std::filesystem::directory_entry& entry);
    std::vector<FileInfo> IterateDirectory(const std::string& directory, std::vector<std::string> extensions = {});
    bool FileExists(const std::string_view name);

    // Rendering
    void UpdateRenderItemAABB(RenderItem& renderItem);
}