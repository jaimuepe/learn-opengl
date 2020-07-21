#version 450 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {

  // ambient light
  float ambientStrength = 0.2;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse light

  vec3 n = normalize(Normal);

  vec3 lightDir = normalize(lightPos - FragPos);

  float diff = max(0.0, dot(n, lightDir));
  vec3 diffuse = diff * lightColor;

  vec3 result = (ambient + diffuse) * objectColor;
  FragColor = vec4(result, 1.0);
}
