#version 330 core

uniform vec4 outColor;
out vec4 color;

void main() {
    color = outColor;
}