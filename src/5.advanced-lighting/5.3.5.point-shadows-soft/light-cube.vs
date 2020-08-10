#version 450 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform Camera {
  vec3 cameraPosition;
  mat4 cameraView;
  mat4 cameraProjection;
};

uniform mat4 model;

void main() {
  gl_Position = cameraProjection * cameraView * model * vec4(aPos, 1.0);
}