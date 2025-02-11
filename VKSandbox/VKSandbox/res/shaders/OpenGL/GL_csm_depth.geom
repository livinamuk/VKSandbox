#version 450 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

readonly restrict layout(std430, binding = 0) buffer lightSpaceMatricesBuffer {
	mat4 lightSpaceMatrices[];
};

void main() {          

	for (int i = 0; i < 3; ++i) {
		gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}  
