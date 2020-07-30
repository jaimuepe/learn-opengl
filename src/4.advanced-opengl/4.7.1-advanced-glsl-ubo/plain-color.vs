#version 450 core

layout(location = 0) in vec3 aPos;

layout(std140, binding = 2) uniform Matrices {
  mat4 view;       // offset             =   0
  float dummy;     // offset = 16N       =  64
  mat4 projection; // offset = 16N +  4N =  80
                   // total  = 20N + 16N = 144
};

uniform mat4 model;

void main() { gl_Position = projection * view * model * vec4(aPos, 1.0); }