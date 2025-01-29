#version 460 core
layout (location = 0) in vec3 vPos;

#include "../common/util.glsl"
#include "../common/types.glsl"

readonly restrict layout(std430, binding = 2) buffer playerDataBuffer {
	PlayerData playerData[];
};

out vec3 TexCoords;

void main() {

    int playerIndex = gl_BaseInstance;
	mat4 projectionView = playerData[playerIndex].projectionView;
	mat4 skyboxModelMatrix = playerData[playerIndex].skyboxModelMatrix;

    TexCoords = vPos;
    gl_Position = projectionView * skyboxModelMatrix * vec4(vPos, 1.0);
}