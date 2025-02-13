#version 460 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 basePosition;
uniform float scale;
uniform int gridSize;

out vec3 Normal;
out vec4 WorldPos;

void main() {
	Normal = vNormal;

    int instanceX = gl_InstanceID % gridSize;
    int instanceZ = gl_InstanceID / gridSize;

    // Compute world position offset
    vec3 instanceOffset = basePosition + vec3(instanceX * scale, 0, instanceZ * scale);

    // Transform vertex position
    WorldPos = vec4(vPosition * scale + instanceOffset, 1.0);

	gl_Position = projection * view *  WorldPos;
}