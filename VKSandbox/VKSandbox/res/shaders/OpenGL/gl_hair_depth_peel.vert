#version 460 core
#include "../common/util.glsl"
#include "../common/types.glsl"
#include "../common/constants.glsl"

layout (location = 0) in vec3 vPosition;

readonly restrict layout(std430, binding = 2) buffer viewportDataBuffer {
	ViewportData viewportData[];
};

layout(std430, binding = 3) readonly buffer renderItemsBuffer {
    RenderItem renderItems[];
};

out vec4 WorldPos;
out mat4 ViewMatrix;

void main() {

    int viewportIndex = gl_BaseInstance >> VIEWPORT_INDEX_SHIFT;
    int instanceOffset = gl_BaseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);

    int globalInstanceIndex = instanceOffset + gl_InstanceID;
    RenderItem renderItem = renderItems[globalInstanceIndex];
    
    mat4 projectionView = viewportData[viewportIndex].projectionView;
    mat4 modelMatrix = renderItem.modelMatrix;
              
    ViewMatrix = viewportData[viewportIndex].view;
    WorldPos = modelMatrix * vec4(vPosition, 1.0);

	gl_Position = projectionView * WorldPos;
}