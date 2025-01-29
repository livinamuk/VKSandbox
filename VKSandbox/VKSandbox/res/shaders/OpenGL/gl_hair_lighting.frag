#version 460 core
#extension GL_ARB_bindless_texture : enable

#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"
#include "../common/types.glsl"
#include "../common/util.glsl"

layout (location = 0) out vec4 FragOut;
layout (location = 1) out vec4 ViewSpaceDepthPreviousOut;
layout (binding = 0) uniform sampler2D ViewSpaceDepthTexture;

readonly restrict layout(std430, binding = 0) buffer textureSamplersBuffer {
	uvec2 textureSamplers[];
};

readonly restrict layout(std430, binding = 1) buffer rendereDataBuffer {
	RendererData rendereData[];
};

readonly restrict layout(std430, binding = 2) buffer playerDataBuffer {
	PlayerData playerData[];
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec3 WorldPos;

uniform vec3 viewPos;
uniform int settings;
uniform float renderTargetWidth;
uniform float renderTargetHeight;
uniform vec2 viewportSize;
uniform vec2 viewportPosition;

uniform int baseColorHandle;
uniform int normalHandle;
uniform int rmaHandle;

void main() {
    vec4 baseColor = texture(sampler2D(textureSamplers[baseColorHandle]), TexCoord);
    vec3 normalMap = texture(sampler2D(textureSamplers[normalHandle]), TexCoord).rgb;   
    vec3 rma = texture(sampler2D(textureSamplers[rmaHandle]), TexCoord).rgb;   
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
        
    lightPosition = (vec3(7, 0, 10) * 0.5) + vec3(-0.5, 0.525, 1);
    lightStrength = 1;
    lightRadius = 10;
    	
    vec3 directLighting = GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal, WorldPos.xyz, baseColor.rgb, roughness, metallic, viewPos);
    
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

    vec2 uv_screenspace = gl_FragCoord.xy / vec2(renderTargetWidth, renderTargetHeight);
    float ViewSpaceDepth = texture2D(ViewSpaceDepthTexture, uv_screenspace).r;
    ViewSpaceDepthPreviousOut = vec4(ViewSpaceDepth, 0, 0, 0);
}
