#version 450 core

in vec3 ourColor;
in vec2 texCoord;

uniform float mixFactor;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 FragColor;

void main() {

  FragColor =
      mix(texture(texture1, texCoord), texture(texture2, texCoord), mixFactor);
}
