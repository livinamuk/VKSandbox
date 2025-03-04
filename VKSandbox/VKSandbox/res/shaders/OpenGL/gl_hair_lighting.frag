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
layout (binding = 4) uniform sampler2D FlashlightCookieTexture;

readonly restrict layout(std430, binding = 1) buffer rendererDataBuffer {
	RendererData rendererData;
};

readonly restrict layout(std430, binding = 4) buffer lightsBuffer {
	Light lights[];
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec4 WorldPos;
in vec3 ViewPos;
in mat4 FlashlightProjectionView;
in vec4 FlashlightDir;
in vec4 FlashlightPosition;
in float FlashlightModifer;
in vec3 CameraForward;

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

    // Direct light
    vec3 directLighting = vec3(0); 
    for (int i = 0; i < 2; i++) {    
        Light light = lights[i];
        vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
        vec3 lightColor =  vec3(light.colorR, light.colorG, light.colorB);
        float lightStrength = light.strength;
        float lightRadius = light.radius;
        directLighting += GetDirectLighting(lightPosition, lightColor, lightRadius, lightStrength, normal.xyz, WorldPos.xyz, baseColor.rgb, roughness, metallic, ViewPos);
    }


    if (FlashlightModifer > 0.1) { 
        // Player flashlight				
		vec3 spotLightPos = FlashlightPosition.xyz;
		spotLightPos -= vec3(0, 0.0, 0);
		vec3 spotLightDir = normalize(spotLightPos - (ViewPos - CameraForward));
        spotLightDir = FlashlightDir.xyz;
        vec3 spotLightColor = vec3(0.9, 0.95, 1.1);
        float fresnelReflect = 1.0;
        float spotLightRadius = 50.0;
        float spotLightStregth = 3.0;        
        float innerAngle = cos(radians(0.0 * FlashlightModifer));
        float outerAngle = cos(radians(30.0));        
        mat4 lightProjectionView = FlashlightProjectionView;
        vec3 cookie = ApplyCookie(lightProjectionView, WorldPos.xyz, spotLightPos, spotLightColor, 10, FlashlightCookieTexture);
        vec3 spotLighting = GetSpotlightLighting(spotLightPos, spotLightDir, spotLightColor, spotLightRadius, spotLightStregth, innerAngle, outerAngle, normal.xyz, WorldPos.xyz, baseColor.rgb, roughness, metallic, ViewPos, lightProjectionView);
        vec4 FragPosLightSpace = lightProjectionView * vec4(WorldPos.xyz, 1.0);
        //float shadow = SpotlightShadowCalculation(FragPosLightSpace, normal.xyz, spotLightDir, WorldPos.xyz, spotLightPos, ViewPos, FlashlighShadowMapTexture);  
        //spotLighting *= vec3(1 - shadow);
        spotLighting *= cookie * cookie * 5 * spotLightColor;
        directLighting += vec3(spotLighting) * FlashlightModifer;
    }
       
    
   vec3 amibentLightColor = vec3(1, 0.98, 0.94);
   float ambientIntensity = 0.05;
   vec3 ambientColor = baseColor.rgb * amibentLightColor;
   vec3 ambientLighting = ambientColor * ambientIntensity;
    
    vec3 finalColor = directLighting.rgb;// + ambientLighting;
    
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
