#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"

layout (location = 0) out vec4 FinalLighting;
layout (location = 1) out vec4 Normal;
layout (binding = 0) uniform samplerCube cubeMap;

in vec3 TexCoords;

readonly restrict layout(std430, binding = 1) buffer rendereDataBuffer {
	RendererData rendererData;
};

void contrastAdjust( inout vec4 color, in float c) {
    float t = 0.5 - c * 0.5;
    color.rgb = color.rgb * c + t;
}

void main() {

    FinalLighting.rgb = texture(cubeMap, TexCoords).rgb;
    FinalLighting.a = 1.0;

    float gBufferWidth = rendererData.gBufferWidth;
    float gBufferHeight = rendererData.gBufferHeight;
    int splitscreenMode = rendererData.splitscreenMode;

    vec2 renderTargetSize = vec2(gBufferWidth, gBufferHeight);
    int playerIndex = GetPlayerIndex(gl_FragCoord.xy, renderTargetSize, splitscreenMode);
  
  // if (playerIndex == 0) {
  // FinalLighting.rgb = vec3(1.0) - FinalLighting.rgb;
  // }
  // if (playerIndex == 1) {
  // FinalLighting.rgb *= vec3(1,0,0);
  // }
  // if (playerIndex == 2) {
  // FinalLighting.rgb *= vec3(0,1,0);
  // }
  // if (playerIndex == 3) {
  // FinalLighting.rgb *= vec3(0,0,1);
  // }
}
