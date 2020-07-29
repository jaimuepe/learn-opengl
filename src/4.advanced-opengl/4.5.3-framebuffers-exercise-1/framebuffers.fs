#version 450 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D texture0;

void main() { FragColor = texture(texture0, TexCoords); }