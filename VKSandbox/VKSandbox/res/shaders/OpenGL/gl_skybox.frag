#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"

layout (location = 0) out vec4 FinalLighting;
layout (location = 1) out vec4 Normal;
layout (binding = 0) uniform samplerCube cubeMap;

in vec3 TexCoords;

void main() {
    FinalLighting.rgb = texture(cubeMap, TexCoords).rgb;
    FinalLighting.a = 1.0;
}
