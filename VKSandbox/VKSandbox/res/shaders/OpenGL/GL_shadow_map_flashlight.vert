#version 460  core
layout (location = 0) in vec3 vPosition;
uniform mat4 ProjectionView;
uniform mat4 modelMatrix;

void main() {
    gl_Position = ProjectionView * modelMatrix * vec4(vPosition, 1.0);
}  