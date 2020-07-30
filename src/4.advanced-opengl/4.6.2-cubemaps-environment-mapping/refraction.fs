#version 450 core

in vec3 Position;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {

  // air -> glass (snell's law)
  float ratio = 1.0 / 1.52;

  vec3 I = normalize(Position - cameraPos);
  vec3 R = refract(I, normalize(Normal), ratio);

  FragColor = texture(skybox, R);
}