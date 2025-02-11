#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
    ViewportData viewportData[];
};

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 modelMatrix;
uniform mat4 inverseModelMatrix;

void main() {
    // Pass UV coordinates (scaled as desired)
    TexCoord = vUV * 50.0;
    
    // Retrieve projection and view info from the viewport buffer.
    int viewportIndex = gl_BaseInstance;
    mat4 projectionView = viewportData[viewportIndex].projectionView;
    
    // Transform the vertex position using the model matrix.
    // This model matrix now incorporates the non-uniform scaling (for example, 0.25 for x/z and 5 for y, etc.)
    vec4 worldPos4 = modelMatrix * vec4(vPosition, 1.0);
    WorldPos = worldPos4.xyz;
    
    // Compute the normal transformation.
    // For non-uniform scaling the normals must be transformed with the transpose of the inverse of the model matrix.
    // Here, inverseModelMatrix is assumed to be the inverse of modelMatrix.
    mat3 normalMatrix = transpose(mat3(inverseModelMatrix));
    Normal = normalize(normalMatrix * vNormal);
    
    // Transform the tangent in the same way.
    Tangent = normalize(normalMatrix * vTangent);
    
    // Compute the bitangent as the cross product of the transformed normal and tangent.
    Bitangent = normalize(cross(Normal, Tangent));
    
    // Finally, compute the clip-space position.
    gl_Position = projectionView * worldPos4;
}
