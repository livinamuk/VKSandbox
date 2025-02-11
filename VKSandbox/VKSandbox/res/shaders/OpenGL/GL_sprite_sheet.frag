#version 460 core

layout (location = 0) out vec4 FragOut;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D Texture;

void main() {

    vec4 color = texture2D(Texture, TexCoord);
    //color = vec4(TexCoord, 0,0);
    FragOut.rgb = color.rgb;
	FragOut.a = 1.0;//color.a;
}
