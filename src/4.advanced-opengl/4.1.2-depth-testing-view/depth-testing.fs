#version 450 core

in vec2 TexCoords;

out vec4 FragColor;

uniform float zFar;
uniform float zNear;

void main() {

  float depth = gl_FragCoord.z;
  // gl_FragCoord is in screenSpace [0-1], need to translate to ndc
  float ndc = 2.0 * depth - 1.0;

  // non-linear depth gives high resolution on low values and lower resolution
  // on higher values
  // depth = (1 / z - 1 / near) / (1 / far - 1 / near)

  float linearDepth =
      (2.0 * zNear * zFar) / (zFar + zNear - ndc * (zFar - zNear));

  // divide by zFar to normalize (not a part of the linealized depth but allows
  // visualization)
  linearDepth /= zFar;

  FragColor = vec4(vec3(linearDepth), 1.0);
}