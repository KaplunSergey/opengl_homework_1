#version 330 core

uniform sampler2D image;

in vec2 outTextureCoordinates;

out vec4 outColor;

void main() {
    outColor = texture(image, outTextureCoordinates);
}