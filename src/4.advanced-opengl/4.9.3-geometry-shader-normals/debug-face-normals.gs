#version 450 core

layout(triangles) in;
layout(line_strip, max_vertices = 8) out;

layout(std140, binding = 0) uniform Matrices {
  mat4 view;
  mat4 projection;
};

// clang-format off

in VS_OUT { 
  vec3 normal; 
} gs_in[];

out GS_OUT {
  vec3 color;
} gs_out;

// clang-format on

void main() {

  // gl_Position is in camera space

  // first we draw the triangles

  gs_out.color = vec3(0.0, 1.0, 0.0);

  for (int i = 0; i < 3; ++i) {

    gl_Position = projection * gl_in[i].gl_Position;
    EmitVertex();

    gl_Position = projection * gl_in[(i + 1) % 3].gl_Position;
    EmitVertex();

    EndPrimitive();
  }

  // now we draw the face normal

  vec4 p0 = gl_in[0].gl_Position;
  vec4 p1 = gl_in[1].gl_Position;
  vec4 p2 = gl_in[2].gl_Position;

  vec3 a = vec3(p0 - p1);
  vec3 b = vec3(p2 - p1);

  vec3 n = normalize(cross(b, a));

  vec4 c = (p0 + p1 + p2) / 3.0;

  gs_out.color = vec3(1.0, 0.0, 0.0);

  gl_Position = projection * c;
  EmitVertex();

  gl_Position = projection * (c + vec4(0.1 * n, 0.0));
  EmitVertex();

  EndPrimitive();
}