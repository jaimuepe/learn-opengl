#version 450 core

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

uniform sampler2D diffuse_texture0;

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

  float spec;

  if (gl_FragCoord.x > 600) {

    // blinn-phong

    vec3 halfwayDir = normalize(fragLightDir + fragCameraDir);
    spec = pow(max(0.0, dot(halfwayDir, normal)), 32.0);

  } else {

    // phong

    vec3 ref = reflect(-fragLightDir, normal);
    spec = pow(max(0.0, dot(fragCameraDir, ref)), 8.0);
  }

  vec3 specular = light.specular * spec * specularColor;

  return attenuation * (ambient + diffuse + specular);
}

void main() {

  vec3 color = vec3(texture(diffuse_texture0, fs_in.texCoords));
  vec3 n = normalize(fs_in.normal);

  vec3 result = vec3(0.0);

  for (int i = 0; i < nPointLights; ++i) {
    result += calculateLight(n, color, vec3(1.0), pointLights[i]);
  }

  FragColor = vec4(result, 1.0);
}