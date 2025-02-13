
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

float Rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}
