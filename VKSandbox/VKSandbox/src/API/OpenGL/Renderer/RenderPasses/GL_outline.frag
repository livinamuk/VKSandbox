#version 460 core
layout (location = 0) out vec4 FragOut;

layout(r8, binding = 0) uniform image2D outlineMask;

uniform int u_offsetCount;
uniform ivec2 u_offsets[48];

flat in int offsetIndex;

void main() {

    ivec2 pixelCoords = ivec2(gl_FragCoord.xy) + u_offsets[offsetIndex];
    float mask = imageLoad(outlineMask, pixelCoords).r;
    FragOut = vec4(mask, 0.0, 0.0, 0.0);
}