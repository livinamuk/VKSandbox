#version 460 core
#include "../common/types.glsl"

layout (location = 0) out vec4 FragOut;
layout(r8, binding = 0) uniform image2D outlineMask;

uniform int u_offsetCount;
uniform int u_viewportIndex;
uniform ivec2 u_offsets[256];

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

flat in int offsetIndex;

void main() {
    ivec2 pixelCoords = ivec2(gl_FragCoord.xy) + u_offsets[offsetIndex];
    
    int xOffset = viewportData[u_viewportIndex].xOffset;
    int yOffset = viewportData[u_viewportIndex].yOffset;
    int viewportWidth = viewportData[u_viewportIndex].width;
    int viewportHeight = viewportData[u_viewportIndex].height;
    
    pixelCoords.x = clamp(pixelCoords.x, xOffset, xOffset + viewportWidth - 1);
    pixelCoords.y = clamp(pixelCoords.y, yOffset, yOffset + viewportHeight - 1);

    float mask = imageLoad(outlineMask, pixelCoords).r;
    FragOut = vec4(mask, 0.0, 0.0, 0.0);
}