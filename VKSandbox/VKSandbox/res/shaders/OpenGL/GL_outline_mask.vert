#version 460 core
#include "../common/types.glsl"
layout (location = 0) in vec3 vPosition;

uniform int u_viewportIndex;
uniform mat4 u_modelMatrix;
uniform ivec2 u_offsets[16];

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

void main() {

	mat4 projectionView = viewportData[u_viewportIndex].projectionView;            
    vec4 WorldPos = u_modelMatrix * vec4(vPosition, 1.0);   
	gl_Position = projectionView * WorldPos;    
}