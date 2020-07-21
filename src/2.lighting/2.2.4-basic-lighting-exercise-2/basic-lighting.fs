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

  // direction fragment->light
  vec3 lightDir = normalize(lightPos - FragPos);

  float diff = max(0.0, dot(n, lightDir));
  vec3 diffuse = diff * lightColor;

  // specular light

  float specularStrength = 0.5;

  // negative! we want light->fragment now to get fragment->light'
  vec3 reflectDir = reflect(-lightDir, n);

  // fragment->eye
  // viewPos at (0, 0, 0) since it's view space
  vec3 viewDir = normalize(-FragPos);

  float spec = pow(max(0.0, dot(reflectDir, viewDir)), 32);

  vec3 specular = spec * lightColor;

  vec3 result = (ambient + diffuse + specular) * objectColor;
  FragColor = vec4(result, 1.0);
}
