#version 460  core
layout (location = 0) in vec3 vPosition;
uniform mat4 u_projectionView;
uniform mat4 u_modelMatrix;

void main() {
    gl_Position = u_projectionView * u_modelMatrix * vec4(vPosition, 1.0);
}  