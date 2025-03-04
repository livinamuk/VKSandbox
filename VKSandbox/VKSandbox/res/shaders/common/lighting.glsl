#include "../common/pbr_functions.glsl"

vec3 GetDirectLighting(vec3 lightPos, vec3 lightColor, float radius, float strength, vec3 Normal, vec3 WorldPos, vec3 baseColor, float roughness, float metallic, vec3 viewPos) {       
    float fresnelReflect = 1.0;
	vec3 viewDir = normalize(viewPos - WorldPos);
	float lightRadiance = strength;
	vec3 lightDir = normalize(lightPos - WorldPos);   
	float lightAttenuation = smoothstep(radius, 0, length(lightPos - WorldPos));
	float irradiance = max(dot(lightDir, Normal), 0.0) ;
	irradiance *= lightAttenuation * lightRadiance;
	vec3 brdf = microfacetBRDF(lightDir, viewDir, Normal, baseColor, metallic, fresnelReflect, roughness);
    return brdf * irradiance * clamp(lightColor, 0, 1);
}

vec3 GetSpotlightLighting2(vec3 lightPos, vec3 lightDir, vec3 lightColor, float radius, float strength, float innerAngle, float outerAngle, vec3 Normal, vec3 WorldPos, vec3 baseColor, float roughness, float metallic, vec3 viewPos) {       
    float fresnelReflect = 1.0;
	vec3 viewDir = normalize(viewPos - WorldPos);
	float lightRadiance = strength;
	vec3 toLight = normalize(lightPos - WorldPos);   
	float lightAttenuation = smoothstep(radius, 0, length(lightPos - WorldPos));

	// Spotlight effect
	float spotIntensity = dot(toLight, normalize(-lightDir));
	float spotFactor = smoothstep(outerAngle, innerAngle, spotIntensity);
	
	float irradiance = max(dot(toLight, Normal), 0.0);
	irradiance *= lightAttenuation * lightRadiance * spotFactor;
	
	vec3 brdf = microfacetBRDF(toLight, viewDir, Normal, baseColor, metallic, fresnelReflect, roughness);
    return brdf * irradiance * clamp(lightColor, 0, 1);
}

vec3 GetSpotlightLighting(
    vec3 lightPos, vec3 lightDir, vec3 lightColor, 
    float radius, float strength, 
    float innerAngle, float outerAngle, 
    vec3 Normal, vec3 WorldPos, 
    vec3 baseColor, float roughness, float metallic, 
    vec3 viewPos, mat4 LightViewProj
) {       
    float fresnelReflect = 1.0;
    vec3 viewDir = normalize(viewPos - WorldPos);
    float lightRadiance = strength;
    vec3 toLight = normalize(lightPos - WorldPos);
    
    // **Compute standard distance attenuation**
    float lightAttenuation = smoothstep(radius, 0, length(lightPos - WorldPos));

    // **Spotlight intensity based on angle**
    float spotIntensity = dot(toLight, normalize(-lightDir));
    float spotFactor = smoothstep(outerAngle, innerAngle, spotIntensity);

    // **Project world position into light space (same as cookie projection)**
    vec4 lightSpacePos = LightViewProj * vec4(WorldPos, 1.0);
    vec2 projectedUV = lightSpacePos.xy / lightSpacePos.w;
    projectedUV = projectedUV * 0.5 + 0.5;

    // **Depth factor to match cookie projection**
    float depthFactor = 1.0 / max(lightSpacePos.w, 0.001);
    projectedUV *= depthFactor; // Perspective warp

    // **Weaken light over distance (same as cookie)**
    float distanceFactor = clamp(1.0 - (length(lightPos - WorldPos) / radius), 0.0, 1.0);
    distanceFactor *= distanceFactor; // Quadratic falloff for smooth fade

    // **Blend spotFactor with perspective-based fade**
    spotFactor *= distanceFactor;

    // **Compute final light intensity**
    float irradiance = max(dot(toLight, Normal), 0.0);
    irradiance *= lightAttenuation * lightRadiance * spotFactor;

    vec3 brdf = microfacetBRDF(toLight, viewDir, Normal, baseColor, metallic, fresnelReflect, roughness);
    return brdf * irradiance * clamp(lightColor, 0, 1);
}

float SpotlightShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, vec3 fragWorldPos, vec3 lightPos, vec3 viewPos, sampler2D shadowMap) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;    
    const float slopeFactor = 0.003;
    float slopeBias = slopeFactor * max(0.0, (1.0 - dot(normal, lightDir)));

    float dist = distance(fragWorldPos, lightPos);
    float scaleFactor = 0.028;
    float epsilon = 0.001;
    float baseBias = 0.0001;
    float bias = baseBias + (scaleFactor / (dist + epsilon));

    // PCF Filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    const int kernelSize = 2;
    for (int x = -kernelSize; x <= kernelSize; ++x) {
        for (int y = -kernelSize; y <= kernelSize; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float closestDepth = texture(shadowMap, projCoords.xy + offset).r;
            shadow += (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= pow((2 * kernelSize + 1), 2);

    return shadow;
}

vec3 ApplyCookie(mat4 LightViewProj, vec3 worldPos, vec3 lightPos, vec3 lightColor, float maxDistance, sampler2D cookieTexture) {
    vec4 lightSpacePos = LightViewProj * vec4(worldPos, 1.0);
    vec2 cookieUV = lightSpacePos.xy / lightSpacePos.w;
    float depthFactor = 1.0 / max(lightSpacePos.w, 0.001);
    cookieUV *= depthFactor;
    cookieUV = cookieUV * 0.5 + 0.5;
    vec2 clampedUV = clamp(cookieUV, vec2(0.0), vec2(1.0));
    float fadeFactor = 1.0 - length(cookieUV - clampedUV) * 15.0;
    fadeFactor = clamp(fadeFactor, 0.0, 1.0);

    // Distance weakening
    float dist = length(worldPos - lightPos);
    float distanceFactor = clamp(1.0 - (dist / maxDistance), 0.0, 1.0);
    distanceFactor = distanceFactor * distanceFactor;

    float cookieFactor = texture(cookieTexture, clampedUV).r;
    return lightColor * cookieFactor * fadeFactor * distanceFactor;
}

//vec3 GetDirectionalLighting(vec3 WorldPos, vec3 Normal, vec3 baseColor, float roughness, float metallic, vec3 viewPos, vec3 lightDir, vec3 lightColor, float strength, float fresnelReflect) {
//	vec3 viewDir = normalize(viewPos - WorldPos);
//	float irradiance = max(dot(lightDir, Normal), 0.0) * strength;
//	vec3 brdf = microfacetBRDF(lightDir, viewDir, Normal, baseColor, metallic, fresnelReflect, roughness);
//    return brdf * irradiance * clamp(lightColor, 0, 1);
//}