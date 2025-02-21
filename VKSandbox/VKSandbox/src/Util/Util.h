#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include "Renderer/Types/Animation.h"
#include <vector>
#include <filesystem>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>

namespace Util {
    // Math
    int RandomInt(int min, int max);
    void NormalizeWeights(std::vector<float>& weights); 
    void InterpolateQuaternion(glm::quat& Out, const glm::quat& Start, const glm::quat& End, float pFactor);
    float FInterpTo(float current, float target, float deltaTime, float interpSpeed);
    float RandomFloat(float min, float max);
    float MapRange(float inValue, float minInRange, float maxInRange, float minOutRange, float maxOutRange);
    glm::ivec2 WorldToScreenCoords(const glm::vec3& worldPos, const glm::mat4& viewProjection, int screenWidth, int screenHeight, bool flipY = false);
    glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b);
    glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b);
    bool IsNan(float value);
    bool IsNan(glm::vec2 value);
    bool IsNan(glm::vec3 value);
    bool IsNan(glm::vec4 value);

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

    // Text
    std::string BoolToString(bool b);
    std::string Vec3ToString(glm::vec3 v);
    std::string Mat4ToString(glm::mat4 m);
    std::string Lowercase(std::string& str);
    std::string Uppercase(std::string& str);
    std::string ViewportModeToString(const ShadingMode& viewportMode);
    std::string CameraViewToString(const CameraView& cameraView);
    std::string EditorStateToString(const EditorState& g_editorState);
    std::string WeaponActionToString(const WeaponAction& weaponAction);
    std::string ImageDataTypeToString(const ImageDataType& imageDataType);

    // File
    const char* CopyConstChar(const char* text);
    bool StrCmp(const char* queryA, const char* queryB);
    std::string GetFilename(const std::string& filepath);
    std::string GetFileName(const std::string& filepath);
    std::string RemoveFileExtension(const std::string& filename);
    std::string GetFullPath(const std::filesystem::directory_entry& entry);
    std::string GetFileName(const std::filesystem::directory_entry& entry);
    std::string GetFileNameWithoutExtension(const std::filesystem::directory_entry& entry);
    std::string GetFileExtension(const std::filesystem::directory_entry& entry);
    std::vector<FileInfo> IterateDirectory(const std::string& directory, std::vector<std::string> extensions = {});
    bool FileExists(const std::string_view name);
    bool RenameFile(const std::string& oldFilePath, const std::string& newFilePath);
    FileInfo GetFileInfoFromPath(const std::string& filepath);

    // Rendering
    void UpdateRenderItemAABB(RenderItem& renderItem);
    AABB ComputeWorldAABB(glm::vec3& localAabbMin, glm::vec3& localAabbMax, glm::mat4& modelMatrix);

    // Animation
    //int FindAnimatedNodeIndex(float AnimationTime, const AnimatedNode* animatedNode);
    const AnimatedNode* FindAnimatedNode(Animation* animation, const char* NodeName);
    void CalcInterpolatedPosition(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedScale(glm::vec3& Out, float AnimationTime, const AnimatedNode* animatedNode);
    void CalcInterpolatedRotation(glm::quat& Out, float AnimationTime, const AnimatedNode* animatedNode);
    float SmoothStep(float t);
    float SmoothStepReverse(float t);
    float SteepSlowDownCurve(float t);
    float EaseOut(float t);
    glm::mat4 Mat4InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    glm::mat4 Mat4InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    glm::mat4 Mat4InitTranslationTransform(float x, float y, float z);
    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
    glm::mat4 aiMatrix3x3ToGlm(const aiMatrix3x3& from); 
    AnimatedTransform BlendTransforms(const AnimatedTransform& transformA, const AnimatedTransform& transformB, float blendFactor);
    AnimatedTransform BlendMultipleTransforms(const std::vector<AnimatedTransform>& transforms, const std::vector<float>& weights);

    // Conversions
    std::string LightTypeToString(LightType);
    LightType StringToLightType(const std::string& str);
    
    // Debug Info
    void PrintDebugInfo(TextureData& textureData);
}