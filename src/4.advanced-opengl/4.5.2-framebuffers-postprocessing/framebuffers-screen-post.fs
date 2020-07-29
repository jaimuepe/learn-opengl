#version 450 core

uniform sampler2D screenTexture;
uniform float mode;

in vec2 TexCoords;
out vec4 FragColor;

const float offset = 1.0 / 300.0;

vec4 inversion() {
  vec3 invColor = 1.0 - vec3(texture(screenTexture, TexCoords));
  return vec4(invColor, 1.0);
}

vec4 grayscale() {

  vec3 texColor = vec3(texture(screenTexture, TexCoords));
  float color = dot(texColor, vec3(0.3, 0.59, 0.11));

  return vec4(color, color, color, 1.0);
}

vec4 kernelFilter(mat3 kernel) {

  // clang-format off
  
  vec2 offsets[9] = vec2[](
      vec2(-offset,  offset),  vec2(0.0,  offset),  vec2(offset,  offset),
      vec2(-offset,     0.0),  vec2(0.0,     0.0),  vec2(offset,     0.0),
      vec2(-offset, -offset),  vec2(0.0, -offset),  vec2(offset, -offset)
  );

  // clang-format on

  vec3 col = vec3(0.0);

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      int idx = 3 * i + j;
      col += vec3(texture(screenTexture, TexCoords.st + offsets[idx])) *
             kernel[i][j];
    }
  }

  return vec4(col, 1.0);
}

vec4 edgeDetection() {

  // clang-format off

  mat3 kernel = mat3(
    1.0,   1.0,   1.0,
    1.0,  -8.0,   1.0,
    1.0,   1.0,   1.0
  );

  // clang-format on

  return kernelFilter(kernel);
}

vec4 blur() {

  // clang-format off

  mat3 kernel = mat3(
    1.0 / 16.0,   2.0 / 16.0,   1.0 / 16.0,
    2.0 / 16.0,   4.0 / 16.0,   2.0 / 16.0,
    1.0 / 16.0,   2.0 / 16.0,   1.0 / 16.0
  );

  // clang-format on

  return kernelFilter(kernel);
}

void main() {

  if (mode == 0.0) {
    // no filter
    FragColor = texture(screenTexture, TexCoords);
  } else if (mode == 1.0) {
    FragColor = inversion();
  } else if (mode == 2.0) {
    FragColor = grayscale();
  } else if (mode == 3.0) {
    FragColor = blur();
  } else if (mode == 4.0) {
    FragColor = edgeDetection();
  }
}