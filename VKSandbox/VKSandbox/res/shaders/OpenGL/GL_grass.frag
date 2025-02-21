#version 460 core

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out uvec2 MousePickOut;
layout (location = 4) out vec4 WorldPositionOut;
layout (location = 5) out vec4 EmissiveOut;

layout (binding = 2) uniform sampler2D NoiseTexture;

in vec3 Normal;
in vec3 WorldPos;

void main() {

    vec2 noiseUV = WorldPos.xz * 0.25;
    float noiseValue = texture(NoiseTexture, noiseUV).r;
    
    float noiseSq = noiseValue * noiseValue;

    vec3 grassColor1 = vec3(0.22, 0.33, 0.18);  // Slightly desaturated green
    vec3 grassColor2 = vec3(0.45, 0.40, 0.12);  // More yellowish, dry grass
    vec3 color = mix(grassColor1, grassColor2, noiseValue);
    color = mix(color, vec3(noiseSq), 0.35);
    
    BaseColorOut = vec4(color, 1.0);
    RMAOut = vec4(0.8, 0.5, 1.0, 0.0);
    MousePickOut = uvec2(0, 0);
    NormalOut = vec4(Normal, 0.0);
    WorldPositionOut = vec4(WorldPos, 1.0);
    EmissiveOut = vec4(0.0, 0.0, 0.0, 0.45);
}
