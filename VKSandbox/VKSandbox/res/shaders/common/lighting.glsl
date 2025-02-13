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