#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform Camera {
  vec3 cameraPosition;
  mat4 cameraView;
  mat4 cameraProjection;
};

uniform mat4 model;

uniform mat4 lightSpaceMatrix;

out VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
  vec4 fragPosLightSpace;
}
vs_out;

void main() {

  vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
  vs_out.normal = transpose(inverse(mat3(model))) * aNormal;
  vs_out.texCoords = aTexCoords;
  vs_out.fragPosLightSpace = lightSpaceMatrix * model * vec4(aPos, 1.0);

  gl_Position = cameraProjection * cameraView * model * vec4(aPos, 1.0);
}