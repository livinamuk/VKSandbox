#version 460 core
layout (location = 0) out vec4 FragOut;
layout (binding = 0) uniform sampler2D Texture;
in vec2 TexCoord;
in vec2 TexCoordNext;
uniform float u_mixFactor;

void main() {
    vec4 color = texture2D(Texture, TexCoord);
    vec4 colorNext = texture2D(Texture, TexCoordNext);
    FragOut = mix(color, colorNext, u_mixFactor);
}
