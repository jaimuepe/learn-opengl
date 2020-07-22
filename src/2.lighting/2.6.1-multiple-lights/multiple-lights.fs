#version 450 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct DirectionalLight {

  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {

  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constantAtt;
  float linearAtt;
  float quadraticAtt;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

uniform Material material;

uniform DirectionalLight dirLight;

#define N_POINT_LIGHTS 4
uniform PointLight pointLights[N_POINT_LIGHTS];

vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragViewDir);

void main() {

  vec3 normal = normalize(Normal);

  vec3 fragViewDir = normalize(viewPos - FragPos);

  vec3 result = vec3(0.0, 0.0, 0.0);

  result += calculateDirLight(dirLight, normal, fragViewDir);

  for (int i = 0; i < N_POINT_LIGHTS; ++i) {
    result += calculatePointLight(pointLights[i], normal, fragViewDir);
  }

  FragColor = vec4(result, 1.0);
}

vec3 calculateDirLight(DirectionalLight light, vec3 normal, vec3 fragViewDir) {

  vec3 lightDir = light.direction;

  // ambient
  vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).xyz;

  // diffuse

  float diff = max(0.0, dot(normal, -lightDir));
  vec3 diffuse =
      diff * light.diffuse * texture(material.diffuse, TexCoords).xyz;

  // specular

  vec3 reflectDir = reflect(lightDir, normal);

  float spec = pow(max(0.0, dot(reflectDir, fragViewDir)), material.shininess);

  vec3 specular =
      spec * light.specular * texture(material.specular, TexCoords).xyz;

  return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragViewDir) {

  float distance = length(light.position - FragPos);

  float attenuation = 1.0 / (light.constantAtt + light.linearAtt * distance +
                             light.quadraticAtt * distance * distance);

  // ambient

  vec3 ambient =
      attenuation * light.ambient * texture(material.diffuse, TexCoords).xyz;

  // diffuse

  vec3 fragLightDir = normalize(light.position - FragPos);

  float diff = max(0.0, dot(normal, fragLightDir));

  vec3 diffuse = attenuation * diff * light.diffuse *
                 texture(material.diffuse, TexCoords).xyz;

  // specular

  vec3 reflectDir = reflect(-fragLightDir, normal);

  float spec = pow(max(0.0, dot(reflectDir, fragViewDir)), material.shininess);

  vec3 specular = attenuation * spec * light.specular *
                  texture(material.specular, TexCoords).xyz;

  return ambient + diffuse + specular;
}