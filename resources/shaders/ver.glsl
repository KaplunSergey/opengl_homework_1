#version 330 core

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoordinates;

out vec2 outTextureCoordinates;

void main() {

    outTextureCoordinates = textureCoordinates;

    gl_Position = projectionMatrix * modelMatrix * vec4(position, 1.0);
}