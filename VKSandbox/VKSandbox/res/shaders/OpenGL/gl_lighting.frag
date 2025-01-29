#version 460 core
#include "../common/lighting.glsl"
#include "../common/post_processing.glsl"

layout (location = 0) out vec4 FragOut;
layout (location = 1) out uvec2  MousePickOut;
layout (binding = 0) uniform sampler2D baseColorTexture;
layout (binding = 1) uniform sampler2D normalTexture;
layout (binding = 2) uniform sampler2D rmaTexture;

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec3 WorldPos;

uniform vec3 viewPos;
uniform int settings;
uniform float viewportWidth;
uniform float viewportHeight;
uniform bool isHair;
uniform float time;

uniform int mousePickType;
uniform int mousePickIndex;

void main() {
    vec4 baseColor = texture2D(baseColorTexture, TexCoord);
    vec3 normalMap = texture2D(normalTexture, TexCoord).rgb;
    vec3 rma = texture2D(rmaTexture, TexCoord).rgb;
	baseColor.rgb = pow(baseColor.rgb, vec3(2.2));

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

    float finalAlpha = baseColor.a;
    
    vec3 finalColor = directLighting.rgb + ambientLighting;

    // Tone mapping
	finalColor = mix(finalColor, Tonemap_ACES(finalColor), 1.0);
	finalColor = pow(finalColor, vec3(1.0/2.2));
	finalColor = mix(finalColor, Tonemap_ACES(finalColor), 0.235);

    finalColor.rgb = finalColor.rgb * finalAlpha;
    FragOut = vec4(finalColor, finalAlpha);
    
    MousePickOut.rg = uvec2(mousePickType, mousePickIndex);
}
