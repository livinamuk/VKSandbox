#version 460 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 clippingPlane;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;

void main() {

    vec4 worldPos = model * vec4(vPosition, 1.0);

	TexCoord = vUV;
    WorldPos = worldPos.xyz;
    
    mat4 normalMatrix = transpose(inverse(model));
    Normal = normalize(normalMatrix * vec4(vNormal, 0)).xyz;
    Tangent = normalize(normalMatrix * vec4(vTangent, 0)).xyz;
    BiTangent = normalize(cross(Normal, Tangent));

    gl_ClipDistance[0] = dot(worldPos, clippingPlane);
	gl_Position = projection * view * worldPos;

}