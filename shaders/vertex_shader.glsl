#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 cubePosition;
uniform mat4 projection;
uniform mat4 view;

void main() {
    vec3 worldPos = aPos + cubePosition;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}