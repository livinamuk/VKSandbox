#version 460 core
#include "../common/types.glsl"
#include "../common/util.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

uniform int u_rowCount;
uniform int u_columnCount;
uniform int u_billboard;
uniform vec4 u_position;
uniform vec4 u_rotation;
uniform vec4 u_scale;
uniform int u_frameIndex;
uniform int u_frameNextIndex;

out vec2 TexCoord;
out vec2 TexCoordNext;

void main() {

    float frameWidth = 1.0 / u_columnCount;
    float frameHeight = 1.0 / u_rowCount;
    
    int frameX = u_frameIndex % u_columnCount;
    int frameY = (u_frameIndex - (u_frameIndex % u_columnCount)) / u_columnCount;
    vec2 frameOffset = vec2(frameX * frameWidth, frameY * frameHeight);
    TexCoord = frameOffset + vTexCoord * vec2(frameWidth, frameHeight);

    int frameNextX = u_frameNextIndex % u_columnCount;
    int frameNextY = (u_frameNextIndex - (u_frameNextIndex % u_columnCount)) / u_columnCount;
    vec2 frameNextOffset = vec2(frameNextX * frameWidth, frameNextY * frameHeight);
    TexCoordNext = frameNextOffset + vTexCoord * vec2(frameWidth, frameHeight);
    
    bool billboard = true;
    int viewportIndex = gl_BaseInstance;
	mat4 projectionView = viewportData[viewportIndex].projectionView;
	mat4 inverseView = viewportData[viewportIndex].inverseView;
    
    vec3 position = u_position.xyz;
    vec3 rotation = u_rotation.xyz;
    vec3 scale = u_scale.xyz;

    mat4 modelMatrix = ToMat4(position, rotation, scale);

    //if (u_billboard == 1) {
        vec3 worldPosition = (modelMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

        // Extract scale from model matrix
        vec3 scale2 = vec3(
            length(modelMatrix[0].xyz),
            length(modelMatrix[1].xyz),
            length(modelMatrix[2].xyz)
        );

        mat4 localMatrix = ToMat4(vec3(0,0,0), rotation, scale);

        // Camera basis vectors
        vec3 cameraRight = normalize(inverseView[0].xyz);
        vec3 cameraUp = normalize(inverseView[1].xyz);
        vec3 cameraForward = normalize(-inverseView[2].xyz);

        // Construct the billboard matrix
        mat4 billboardMatrix = mat4(1.0);
        billboardMatrix[0] = vec4(cameraRight, 0.0);
        billboardMatrix[1] = vec4(cameraUp, 0.0);
        billboardMatrix[2] = vec4(cameraForward, 0.0);
        billboardMatrix[3] = vec4(worldPosition, 1.0);

        modelMatrix = billboardMatrix * localMatrix;
  //  }


    gl_Position = projectionView * modelMatrix * vec4(vPosition, 1.0);
}
