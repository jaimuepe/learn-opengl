
#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {
  vec4 tex = texture(screenTexture, TexCoords);
  float c = (tex.r + tex.g + tex.a) * 0.333;
  FragColor = vec4(c, c, c, 1.0);
}