#version 460 core

#ifndef ENABLE_BINDLESS
    #define ENABLE_BINDLESS 1
#endif

#include "../common/util.glsl"
#include "../common/types.glsl"
#include "../common/constants.glsl"

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

layout(std430, binding = 3) readonly buffer renderItemsBuffer {
    RenderItem renderItems[];
};

out vec2 TexCoord;
out vec4 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec3 ViewPos;
out vec3 EmissiveColor;

out flat int MousePickType;
out flat int MousePickIndex;

#if ENABLE_BINDLESS
out flat int BaseColorTextureIndex;
out flat int NormalTextureIndex;
out flat int RMATextureIndex;
#else
uniform int u_viewportIndex;
uniform int u_globalInstanceIndex;
#endif

void main() {

#if ENABLE_BINDLESS
    int viewportIndex = gl_BaseInstance >> VIEWPORT_INDEX_SHIFT;
    int instanceOffset = gl_BaseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);
    int globalInstanceIndex = instanceOffset + gl_InstanceID;
    
    BaseColorTextureIndex =  renderItems[globalInstanceIndex].baseColorTextureIndex;
	NormalTextureIndex =  renderItems[globalInstanceIndex].normalMapTextureIndex;
	RMATextureIndex =  renderItems[globalInstanceIndex].rmaTextureIndex;   

#else
    int globalInstanceIndex = u_globalInstanceIndex;
    int viewportIndex = u_viewportIndex;
#endif

    RenderItem renderItem = renderItems[globalInstanceIndex]; 
    mat4 modelMatrix = renderItem.modelMatrix;
    mat4 inverseModelMatrix = renderItem.inverseModelMatrix;  
	mat4 projectionView = viewportData[viewportIndex].projectionView;            
    mat4 normalMatrix = transpose(inverseModelMatrix);

    Normal = normalize(normalMatrix * vec4(vNormal, 0)).xyz;
    Tangent = normalize(normalMatrix * vec4(vTangent, 0)).xyz;
    BiTangent = normalize(cross(Normal, Tangent));
    
	TexCoord = vUV;
    WorldPos = modelMatrix * vec4(vPosition, 1.0);
    ViewPos = viewportData[viewportIndex].inverseView[3].xyz;

	MousePickType =  renderItems[globalInstanceIndex].mousePickType;
	MousePickIndex =  renderItems[globalInstanceIndex].mousePickIndex;
        
    EmissiveColor.r = renderItems[globalInstanceIndex].emissiveR;
    EmissiveColor.g = renderItems[globalInstanceIndex].emissiveG;
    EmissiveColor.b = renderItems[globalInstanceIndex].emissiveB;

	gl_Position = projectionView * WorldPos;
}