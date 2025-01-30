#version 460 core
#extension GL_ARB_bindless_texture : enable
#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"

layout (location = 0) out vec4 FragOut;
layout (location = 1) out uvec2  MousePickOut;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 ViewPos;

in flat int BaseColorTextureIndex;
in flat int NormalTextureIndex;
in flat int RMATextureIndex;
in flat int MousePickType;
in flat int MousePickIndex;

readonly restrict layout(std430, binding = 0) buffer textureSamplersBuffer {
	uvec2 textureSamplers[];
};

void main() {
    vec4 baseColor = texture(sampler2D(textureSamplers[BaseColorTextureIndex]), TexCoord);
    vec3 normalMap = texture(sampler2D(textureSamplers[NormalTextureIndex]), TexCoord).rgb;   
    vec3 rma = texture(sampler2D(textureSamplers[RMATextureIndex]), TexCoord).rgb;  
	baseColor.rgb = pow(baseColor.rgb, vec3(2.2));

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

    float finalAlpha = baseColor.a;
    
    vec3 finalColor = directLighting.rgb + ambientLighting;

    // Tone mapping
	finalColor = mix(finalColor, Tonemap_ACES(finalColor), 1.0);
	finalColor = pow(finalColor, vec3(1.0/2.2));
	finalColor = mix(finalColor, Tonemap_ACES(finalColor), 0.235);

    finalColor.rgb = finalColor.rgb * finalAlpha;
    FragOut = vec4(finalColor, finalAlpha);
    
    MousePickOut.rg = uvec2(MousePickType, MousePickIndex);
}
