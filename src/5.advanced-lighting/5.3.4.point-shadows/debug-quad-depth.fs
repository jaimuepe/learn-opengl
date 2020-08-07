#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthMap;

uniform float far;
uniform float near;

float linearizeDepth(float depth) {
  // back to NDC
  float z = depth * 2.0 - 1.0;
  return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
  float depth = texture(depthMap, TexCoords).r;
  // no need to linearize! directional light has no perspective division
  // FragColor = vec4(vec3(linearizeDepth(depth)), 1.0);
  FragColor = vec4(vec3(depth), 1.0);
}