#version 460 core

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

#if ENABLE_BINDLESS
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
#include "../common/types.glsl"
#include "../common/util.glsl"

layout (location = 0) out vec4 FragOut;
layout (location = 1) out vec4 ViewSpaceDepthPreviousOut;
layout (binding = 3) uniform sampler2D ViewSpaceDepthTexture;

readonly restrict layout(std430, binding = 1) buffer rendererDataBuffer {
	RendererData rendererData;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 ViewPos;

in flat int MousePickType;
in flat int MousePickIndex;

void main() {
#if ENABLE_BINDLESS
    vec4 baseColor = texture(sampler2D(textureSamplers[BaseColorTextureIndex]), TexCoord);
    vec3 normalMap = texture(sampler2D(textureSamplers[NormalTextureIndex]), TexCoord).rgb;   
    vec3 rma = texture(sampler2D(textureSamplers[RMATextureIndex]), TexCoord).rgb;  
#else
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
#endif

	baseColor.rgb = pow(baseColor.rgb, vec3(2.2));
    float finalAlpha = baseColor.a;// * 1.25;

    mat3 tbn = mat3(Tangent, BiTangent, Normal);
    vec3 normal = normalize(tbn * (normalMap.rgb * 2.0 - 1.0));
    
    float roughness = rma.r;
    float metallic = rma.g;
    
    vec3 lightPosition = (vec3(7, 0, 10) * 0.5) + vec3(0, 0.125, 1);
    vec3 lightColor = vec3(1, 0.98, 0.94);
    float lightRadius = 5;
    float lightStrength = 2;
        
    lightPosition = vec3(-3.5, 2, 1);
    lightStrength = 1;
    lightRadius = 15;

    vec3 directLighting = GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal, WorldPos.xyz, baseColor.rgb, roughness, metallic, ViewPos);
    
    float ambientIntensity = 0.05;
    vec3 ambientColor = baseColor.rgb * lightColor;
    vec3 ambientLighting = ambientColor * ambientIntensity;
    
    vec3 finalColor = directLighting.rgb + ambientLighting;
    
    // Tone mapping
    finalColor = mix(finalColor, Tonemap_ACES(finalColor), 1.0);
    finalColor = pow(finalColor, vec3(1.0/2.2));
    finalColor = mix(finalColor, Tonemap_ACES(finalColor), 0.235);
    
    finalColor.rgb = finalColor.rgb * finalAlpha;
    FragOut = vec4(finalColor, finalAlpha);

    vec2 uv_screenspace = gl_FragCoord.xy / vec2(rendererData.hairBufferWidth, rendererData.hairBufferHeight);
    float ViewSpaceDepth = texture2D(ViewSpaceDepthTexture, uv_screenspace).r;
    ViewSpaceDepthPreviousOut = vec4(ViewSpaceDepth, 0, 0, 0);
}
