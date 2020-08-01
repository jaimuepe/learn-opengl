#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;
// since it's a matrix and vertex attributes can't be bigger than a vec4, we
// need to use 4 slots (loc 3, 4, 5, 6)
layout(location = 3) in mat4 instanceMatrix;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

out VS_OUT { vec2 texCoords; }
vs_out;

void main() {
  vs_out.texCoords = aTexCoords;
  gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}