#version 460 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 Color;
out vec2 TexCoord;

void main() {
	TexCoord = vTexCoord;
	gl_Position = projection * view * model * vec4(vPosition, 1.0);
}