#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"

layout (location = 0) in vec3 vPos;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

out vec3 TexCoords;

void main() {
    TexCoords = vPos;
    int viewportIndex = gl_BaseInstance;
	mat4 projectionView = viewportData[viewportIndex].skyboxProjectionView;	
    vec3 viewPos = viewportData[viewportIndex].inverseView[3].xyz;
    gl_Position = projectionView * vec4((vPos * 8) + viewPos, 1.0);
}