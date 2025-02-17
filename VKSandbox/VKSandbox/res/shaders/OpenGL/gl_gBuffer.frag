#version 460 core

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

#if ENABLE_BINDLESS == 1
    #extension GL_ARB_bindless_texture : enable        
    readonly restrict layout(std430, binding = 0) buffer textureSamplersBuffer {
	    uvec2 textureSamplers[];
    };    
    in flat int BaseColorTextureIndex;
    in flat int NormalTextureIndex;
    in flat int RMATextureIndex;

#else
    layout (binding = 0) uniform sampler2D baseColorTexture;
    layout (binding = 1) uniform sampler2D normalTexture;
    layout (binding = 2) uniform sampler2D rmaTexture;
#endif

#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"

layout (location = 0) out vec4 BaseColorOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 RMAOut;
layout (location = 3) out uvec2 MousePickOut;
layout (location = 4) out vec4 WorldPositionOut;
layout (location = 5) out vec4 EmissiveOut;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 ViewPos;
in vec3 EmissiveColor;

in flat int MousePickType;
in flat int MousePickIndex;

void main() {
#if ENABLE_BINDLESS == 1
    vec4 baseColor = texture(sampler2D(textureSamplers[BaseColorTextureIndex]), TexCoord);
    vec3 normalMap = texture(sampler2D(textureSamplers[NormalTextureIndex]), TexCoord).rgb;   
    vec3 rma = texture(sampler2D(textureSamplers[RMATextureIndex]), TexCoord).rgb;  
#else
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
#endif

    mat3 tbn = mat3(normalize(Tangent), normalize(BiTangent), normalize(Normal));
    normalMap.rgb = normalMap.rgb * 2.0 - 1.0;
    normalMap = normalize(normalMap);
    vec3 normal = normalize(tbn * (normalMap));
    
    BaseColorOut = vec4(baseColor);
    NormalOut = vec4(normal, 1.0);   
    RMAOut = vec4(rma, 1.0);
    MousePickOut.rg = uvec2(MousePickType, MousePickIndex);
    WorldPositionOut = vec4(WorldPos.rgb, 1.0);

    EmissiveOut = vec4(EmissiveColor, 0);
}
