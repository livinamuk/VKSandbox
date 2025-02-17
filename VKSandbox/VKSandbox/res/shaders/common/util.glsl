
mat4 ToMat4(vec3 position, vec3 rotation, vec3 scale) {
    // Translation matrix
    mat4 translationMatrix = mat4(1.0);
    translationMatrix[3] = vec4(position, 1.0);

    // Rotation matrices (XYZ Euler Order)
    float cosX = cos(rotation.x), sinX = sin(rotation.x);
    float cosY = cos(rotation.y), sinY = sin(rotation.y);
    float cosZ = cos(rotation.z), sinZ = sin(rotation.z);

    mat4 rotX = mat4(
        1,  0,    0,   0,
        0,  cosX, -sinX, 0,
        0,  sinX, cosX, 0,
        0,  0,    0,   1
    );

    mat4 rotY = mat4(
        cosY,  0, sinY,  0,
        0,     1, 0,     0,
        -sinY, 0, cosY,  0,
        0,     0, 0,     1
    );

    mat4 rotZ = mat4(
        cosZ, -sinZ, 0, 0,
        sinZ, cosZ,  0, 0,
        0,    0,     1, 0,
        0,    0,     0, 1
    );

    // Combined rotation (Z * Y * X order)
    mat4 rotationMatrix = rotZ * rotY * rotX;

    // Scale matrix
    mat4 scaleMatrix = mat4(1.0);
    scaleMatrix[0][0] = scale.x;
    scaleMatrix[1][1] = scale.y;
    scaleMatrix[2][2] = scale.z;

    // Final transformation matrix
    return translationMatrix * rotationMatrix * scaleMatrix;
}

float RandOLD(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float Rand(vec2 seed) {
    return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 WorldToScreen(vec3 worldPos, mat4 projView, vec2 viewportPosition, vec2 viewportSize) {
    vec4 clipSpace = projView * vec4(worldPos, 1.0);
    vec3 ndc = clipSpace.xyz / clipSpace.w; // Perspective divide

    // Convert from NDC (-1 to 1) to normalized screen UVs (0 to 1)
    vec2 screenUV = ndc.xy * 0.5 + 0.5;

    // 🔥 Ensure precise viewport size scaling
    return screenUV * viewportSize + viewportPosition;
}

float LinearizeDepth(float nonLinearDepth, float near, float far) {
    float z = nonLinearDepth * 2.0 - 1.0;  // Convert [0,1] range to [-1,1] (NDC space)
    return (2.0 * near * far) / (far + near - z * (far - near)); // Convert to linear depth
}

// Rotation matrix around the X axis.
mat3 RotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}

// Rotation matrix around the Y axis
mat3 RotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}

// Rotation matrix around the Z axis
mat3 RotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1)
    );
}