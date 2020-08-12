#version 450 core

const int MAX_POINT_LIGHTS = 6;

struct PointLight {

  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constantAtt;
  float linearAtt;
  float quadraticAtt;

  float zNear;
  float zFar;
};

in VS_OUT {

  vec2 texCoords;

  vec3 tangentFragPos;
  vec3 tangentLightPos[MAX_POINT_LIGHTS];
  vec3 tangentViewPos;
}
fs_in;

out vec4 FragColor;

uniform int nPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D diffuse_texture0;
uniform sampler2D specular_texture0;
uniform sampler2D normalMap;

uniform samplerCube pointLightShadowMaps[MAX_POINT_LIGHTS];

float calculatePointLightShadow(int lightIndex) {

  vec3 lightToFrag = fs_in.tangentFragPos - fs_in.tangentLightPos[lightIndex];
  float lightToFragLength = length(lightToFrag);

  // normalized [0, 1]
  float currentDepth = lightToFragLength / (pointLights[lightIndex].zFar -
                                            pointLights[lightIndex].zNear);

  if (currentDepth > 1.0) {
    return 0.0;
  }

  // clang-format off

  vec3 sampleOffsetDirections[20] =  vec3[](
      vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
      vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
      vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
      vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
      vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
  );

  // clang-format on

  float viewDistance =
      length(fs_in.tangentViewPos - fs_in.tangentFragPos) /
      (pointLights[lightIndex].zFar - pointLights[lightIndex].zNear);

  // scale the radius based on the distance from the camera (further fragments
  // are more smoothed to avoid edges)
  float diskRadius = (1.0 + viewDistance) / (25.0);

  float shadow = 0.0;
  float bias = 0.005;

  int samples = 20;

  for (int i = 0; i < samples; ++i) {
    float depth = texture(pointLightShadowMaps[lightIndex],
                          lightToFrag + sampleOffsetDirections[i] * diskRadius)
                      .r;

    shadow += currentDepth - bias > depth ? 1.0 : 0.0;
  }

  shadow /= samples;

  return shadow;
}

vec3 calculatePointLight(vec3 normal, vec3 diffuseColor, vec3 specularColor,
                         int index) {

  PointLight light = pointLights[index];

  vec3 fragLightDir =
      normalize(fs_in.tangentLightPos[index] - fs_in.tangentFragPos);
  float distance = length(fragLightDir);

  float attenuation = 1.0 / (light.constantAtt + distance * light.linearAtt +
                             distance * distance * light.quadraticAtt);

  // ambient
  vec3 ambient = light.ambient * diffuseColor;

  // diffuse

  float diff = max(0.0, dot(fragLightDir, normal));
  vec3 diffuse = diff * light.diffuse * diffuseColor;

  // specular

  vec3 fragCameraDir = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);

  // blinn-phong

  vec3 halfwayDir = normalize(fragLightDir + fragCameraDir);
  float spec = pow(max(0.0, dot(halfwayDir, normal)), 32.0);

  vec3 specular = light.specular * spec * specularColor;

  float shadow = calculatePointLightShadow(index);

  return attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

void main() {

  vec3 diffColor = vec3(texture(diffuse_texture0, fs_in.texCoords));
  vec3 specColor = vec3(texture(specular_texture0, fs_in.texCoords));

  // normal in range [0, 1]
  vec3 normal = texture(normalMap, fs_in.texCoords).rgb;
  // transform to [-1, 1]
  vec3 n = normalize(2.0 * normal - 1.0);

  vec3 result = vec3(0.0);

  for (int i = 0; i < nPointLights; ++i) {
    result += calculatePointLight(n, diffColor, specColor, i);
  }

  FragColor = vec4(result, 1.0);
}