#version 450 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main() {
  TexCoords = aPos;
  gl_Position = projection * view * vec4(aPos, 1.0);
  // force z = 1.0 after perspective division so the skybox will always be in
  // the background
  // this allow us to render the skybox last and save some fragment computing
  gl_Position.z = gl_Position.w;
}