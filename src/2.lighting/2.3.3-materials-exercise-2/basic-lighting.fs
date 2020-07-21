#version 450 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;

uniform vec3 lightColor;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main() {

  // ambient light
  vec3 ambient = light.ambient * material.ambient;

  // diffuse light

  vec3 n = normalize(Normal);

  // direction fragment->light
  vec3 lightDir = normalize(light.position - FragPos);

  float diff = max(0.0, dot(n, lightDir));
  vec3 diffuse = diff * light.diffuse * material.diffuse;

  // specular light

  float specularStrength = 0.5;

  // negative! we want light->fragment now to get fragment->light'
  vec3 reflectDir = reflect(-lightDir, n);

  // fragment->eye
  vec3 viewDir = normalize(viewPos - FragPos);

  float spec = pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);

  vec3 specular = spec * light.specular * material.specular;

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}
