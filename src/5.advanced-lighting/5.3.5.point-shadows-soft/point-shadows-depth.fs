#version 450 core

in vec4 FragPos;

uniform vec3 lightPos;

uniform float zNear;
uniform float zFar;

void main() {
  // we are going to manually calculate the depth in linear space, for
  // simplicity
  float lightDistance = length(FragPos.xyz - lightPos);

  // Normalizing [0, 1]
  lightDistance /= (zFar - zNear);

  gl_FragDepth = lightDistance;
}