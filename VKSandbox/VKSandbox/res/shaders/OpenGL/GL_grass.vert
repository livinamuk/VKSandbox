#version 460 core

uniform mat4 projectionView;

out vec3 Normal;
out vec3 WorldPos;

struct Vertex {
    float posX;
    float posY;
    float posZ;
    float normX;
    float normY;
    float normZ;
};

layout(std430, binding = 9) buffer bladePositions {
    vec4 BladePositions[];
};

layout(std430, binding = 10) buffer inputVertexBuffer {
    Vertex InputVertexBuffer[];
};

layout(std430, binding = 11) buffer inputIndexBuffer {
    uint InputIndexBuffer[];
};

uint HashMix(vec2 v) {
    uint x = floatBitsToUint(v.x);
    uint y = floatBitsToUint(v.y);    
    x ^= (x >> 17);
    y ^= (y << 13);
    x *= 374761393u;
    y *= 668265263u;
    x ^= (x >> 15);
    y ^= (y << 17);    
    return x ^ y;
}

void main() {

    const uint indicesPerBlade = 24u;
    const uint hashMod = 360u;
    const uint verticesPerBlade = 12u;

    const uint basePosIndex = gl_VertexID / indicesPerBlade;
    const vec4 basePos = BladePositions[basePosIndex];

    const uint hashVal = HashMix(vec2(basePos.z, basePos.x));
    const uint baseVertex = (hashVal % hashMod) * verticesPerBlade;
    
    const uint index = baseVertex + (gl_VertexID % indicesPerBlade);
    const uint vertex = InputIndexBuffer[index];
    const Vertex v = InputVertexBuffer[vertex];

    WorldPos = vec3(v.posX, v.posY, v.posZ) + basePos.xyz;
    Normal = vec3(v.normX, v.normY, v.normZ);

	gl_Position = projectionView * vec4(WorldPos, 1.0);
}