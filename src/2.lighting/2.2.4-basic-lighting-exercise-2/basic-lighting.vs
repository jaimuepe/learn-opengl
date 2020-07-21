#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 invViewModel;

out vec3 FragPos;
out vec3 Normal;

void main() {

  FragPos = vec3(view * model * vec4(aPos, 1.0));
  Normal = mat3(transpose(invViewModel)) * aNormal;

  gl_Position = projection * view * model * vec4(aPos, 1.0);
}