#version 460 core
#include "../common/types.glsl"
#include "../common/util.glsl"

layout (location = 0) out vec4 FragOut;
layout (binding = 3) uniform sampler2D previousDepthTexture;

readonly restrict layout(std430, binding = 1) buffer rendereDataBuffer {
	RendererData rendereData;
};

in vec4 WorldPos;
in mat4 ViewMatrix;

void main() { 
    vec2 uv_screenspace = gl_FragCoord.xy / vec2(rendereData.hairBufferWidth, rendereData.hairBufferHeight);
    float previousDepth = texture2D(previousDepthTexture, uv_screenspace).r;

    float viewspaceDepth = (ViewMatrix * WorldPos).z;
    float normalizedDepth = (viewspaceDepth - (-rendereData.farPlane)) / ((-rendereData.nearPlane) - (-rendereData.farPlane));    

    if (normalizedDepth >= previousDepth) {
        discard;
    }
    FragOut.r = normalizedDepth;
}
