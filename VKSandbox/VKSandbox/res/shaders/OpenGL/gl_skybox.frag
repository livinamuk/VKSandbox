#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"
#include "../common/post_processing.glsl"

layout (location = 0) out vec4 FinalLighting;
layout (location = 1) out vec4 Normal;
layout (binding = 0) uniform samplerCube cubeMap;

in vec3 TexCoords;

void main() {

    vec3 color = texture(cubeMap, TexCoords).rgb;
    //color = AdjustHue(color, -180);
    //color = AdjustSaturation(color, -0.5);
    color = AdjustLightness(color, -0.85);
    FinalLighting = vec4(color, 1.0);
}
