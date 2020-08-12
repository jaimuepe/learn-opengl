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

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(std140, binding = 0) uniform Camera {
  vec3 cameraPosition;
  mat4 cameraView;
  mat4 cameraProjection;
};

uniform int nPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform mat4 model;

out VS_OUT {

  vec2 texCoords;

  vec3 tangentFragPos;
  vec3 tangentLightPos[MAX_POINT_LIGHTS];
  vec3 tangentViewPos;
}
vs_out;

void main() {

  mat3 invModel = transpose(inverse(mat3(model)));

  vec3 T = normalize(invModel * aTangent);
  vec3 B = normalize(invModel * aBitangent);
  vec3 N = normalize(invModel * aNormal);

  mat3 TBN = mat3(T, B, N);
  mat3 invTBN = transpose(TBN);

  vs_out.texCoords = aTexCoords;

  vs_out.tangentFragPos = invTBN * vec3(model * vec4(aPos, 1.0));

  for (int i = 0; i < nPointLights; ++i) {
    vs_out.tangentLightPos[i] = invTBN * pointLights[i].position;
  }

  vs_out.tangentViewPos = invTBN * cameraPosition;

  gl_Position = cameraProjection * cameraView * model * vec4(aPos, 1.0);
}