#version 460 core

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out uvec2 MousePickOut;
layout (location = 4) out vec4 WorldPositionOut;
layout (location = 5) out vec4 EmissiveOut;

layout (binding = 0) uniform sampler2D NoiseTexture;

in vec3 Normal;
in vec4 WorldPos;

void main() {

    vec2 noiseUV = WorldPos.xz * 0.75;
    float noiseValue = texture(NoiseTexture, noiseUV).r;
    noiseValue = noiseValue * noiseValue;
    //noiseValue = mix(noiseValue, noiseValue * noiseValue, 0.75);
    vec3 grassColor1 = vec3(0.48, 0.749, 0.212); // Dark green
    vec3 grassColor2 = vec3(0.624, 0.749, 0.212); // Light green
    vec3 color = mix(grassColor1, grassColor2, noiseValue);

    BaseColorOut = vec4(0.18, 0.45, 0.12, 1.0);
    BaseColorOut.rgb *= 1.25;
    BaseColorOut = vec4(color, 1.0);
    BaseColorOut = vec4(noiseValue, noiseValue ,noiseValue, 1.0);

    BaseColorOut.rgb = mix(BaseColorOut.rgb, color, 0.25);
    
    //BaseColorOut.rgb += vec3(0.18, 0.35, 0.12);


    RMAOut = vec4(0.8, 0.01, 1, 0);
    MousePickOut = uvec2(0,0);
    NormalOut = vec4(Normal, 0);
    //NormalOut = vec4(1, 0, 0 , 1);
    WorldPositionOut = vec4(WorldPos.xyz, 1.0);
    EmissiveOut = vec4(0, 0, 0, 0);
}
