#version 450 core

struct DirLight {

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

in VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
}
fs_in;

out vec4 FragColor;

const int MAX_POINT_LIGHTS = 4;

uniform int nPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform DirLight dirLight;

uniform int blinnPhong;

uniform sampler2D diffuse_texture0;
uniform sampler2D specular_texture0;

vec3 calculateDirLight(vec3 normal, vec3 diffuseColor, vec3 specularColor) {

  vec3 fragLightDir = -normalize(dirLight.direction);

  // ambient
  vec3 ambient = dirLight.ambient * diffuseColor;

  // diffuse

  float diff = max(0.0, dot(fragLightDir, normal));
  vec3 diffuse = diff * dirLight.diffuse * diffuseColor;

  // specular (blinn-phong)

  vec3 fragCameraDir =
      normalize(-fs_in.fragPos); // cameraPos = 0.0 in camera space

  vec3 halfwayDir = normalize(fragLightDir + fragCameraDir);
  float spec = pow(max(0.0, dot(halfwayDir, normal)), 64.0);

  vec3 specular = dirLight.specular * spec * specularColor;

  return ambient + diffuse + specular;
}

vec3 calculateLight(vec3 normal, vec3 diffuseColor, vec3 specularColor,
                    PointLight light) {

  float distance = length(light.position - fs_in.fragPos);

  float attenuation = 1.0 / (light.constantAtt + distance * light.linearAtt +
                             distance * distance * light.quadraticAtt);

  // ambient
  vec3 ambient = light.ambient * diffuseColor;

  // diffuse

  vec3 fragLightDir = normalize(light.position - fs_in.fragPos);

  float diff = max(0.0, dot(fragLightDir, normal));
  vec3 diffuse = diff * light.diffuse * diffuseColor;

  // specular

  vec3 fragCameraDir =
      normalize(-fs_in.fragPos); // cameraPos = 0.0 in camera space

  // blinn-phong

  vec3 halfwayDir = normalize(fragLightDir + fragCameraDir);
  float spec = pow(max(0.0, dot(halfwayDir, normal)), 64.0);

  vec3 specular = light.specular * spec * specularColor;

  return attenuation * (ambient + diffuse + specular);
}

void main() {

  vec3 diffColor = vec3(texture(diffuse_texture0, fs_in.texCoords));
  vec3 specColor = vec3(texture(specular_texture0, fs_in.texCoords));

  vec3 n = normalize(fs_in.normal);

  vec3 result = vec3(0.0);

  result += calculateDirLight(n, diffColor, specColor);

  for (int i = 0; i < nPointLights; ++i) {
    result += calculateLight(n, diffColor, specColor, pointLights[i]);
  }

  FragColor = vec4(result, 1.0);
}