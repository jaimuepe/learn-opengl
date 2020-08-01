#version 450 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

// per-instance
layout(location = 2) in vec2 aOffset;

out vec3 Color;

void main() {
  Color = aColor;
  gl_Position = vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, 0.0, 1.0);
}