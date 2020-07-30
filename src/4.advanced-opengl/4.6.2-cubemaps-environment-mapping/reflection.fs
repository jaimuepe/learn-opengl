#version 450 core

in vec3 Position;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {

  vec3 I = normalize(Position - cameraPos);
  vec3 R = reflect(I, Normal);

  FragColor = texture(skybox, R);
}