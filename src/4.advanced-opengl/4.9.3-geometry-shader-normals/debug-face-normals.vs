#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

out VS_OUT {
  vec3 normal;
} vs_out;

uniform vec3 cameraPos;

uniform mat4 model;

void main() {
  mat3 normalMatrix = mat3(transpose(inverse(view * model)));
  vs_out.normal = normalMatrix * aNormal;
  gl_Position = view * model * vec4(aPos, 1.0);
}