#version 450 core

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 FragColor;

void main() {

  vec4 tex1 = texture(texture1, texCoord);
  vec4 tex2 = texture(texture2, vec2(1.0 - texCoord.x, texCoord.y));

  FragColor = mix(tex1, tex2, 0.2);
}
