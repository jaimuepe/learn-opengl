#version 450 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {

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
uniform Light light;

void main() {

  vec3 diffuseTex = texture(material.diffuse, TexCoords).xyz;
  vec3 specularTex = texture(material.specular, TexCoords).xyz;

  float distance = length(light.position - FragPos);
  float attenuation = 1.0 / (light.constantAtt + light.linearAtt * distance +
                              light.quadraticAtt * distance * distance);

  // ambient light
  vec3 ambient = attenuation * light.ambient * diffuseTex;

  // diffuse light

  vec3 n = normalize(Normal);

  // direction fragment->light
  vec3 lightDir = normalize(light.position - FragPos);

  float diff = max(0.0, dot(n, lightDir));
  vec3 diffuse = attenuation * diff * light.diffuse * diffuseTex;

  // specular light

  // negative! we want light->fragment now to get fragment->light'
  vec3 reflectDir = reflect(-lightDir, n);

  // fragment->eye
  vec3 viewDir = normalize(viewPos - FragPos);

  float spec = pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);

  vec3 specular = attenuation * spec * light.specular * specularTex;

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}
