#version 460 core

layout (location = 0) out vec4 FragOut;
layout (location = 1) out uvec2  MousePickOut;

uniform vec3 color;
uniform int mousePickType;
uniform int mousePickIndex;

void main() {
    FragOut.rgb = color;
	FragOut.a = 1.0;    
    MousePickOut.rg = uvec2(mousePickType, mousePickIndex);
}
