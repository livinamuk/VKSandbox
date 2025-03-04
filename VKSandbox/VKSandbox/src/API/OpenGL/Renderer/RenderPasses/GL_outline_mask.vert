#version 460 core
#include "../common/types.glsl"
layout (location = 0) in vec3 vPosition;

uniform mat4 u_modelMatrix;
uniform ivec2 u_offsets[48];

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

void main() {

    int viewportIndex = 0;
	mat4 projectionView = viewportData[viewportIndex].projectionView;            
    vec4 WorldPos = u_modelMatrix * vec4(vPosition, 1.0);
    
	//gl_Position = projectionView * vec4(vPosition, 1.0);    
	gl_Position = projectionView * WorldPos;    
    //const float pixelWidth = 1.0 / 1920.0;
    //const float pixelHeight = 1.0 / 1080.0;
    //int offsetX = u_offsets[gl_InstanceID].x;
	//int offsetY = u_offsets[gl_InstanceID].y;
	//gl_Position.x += pixelWidth * gl_Position.z * offsetX;
	//gl_Position.y += pixelHeight * gl_Position.z * offsetY;
}