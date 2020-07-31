#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 1) uniform Time {
  float time;
};

in VS_OUT {
  vec2 texCoords;
} gs_in[];

out GS_OUT {
  vec2 texCoords;
} gs_out;

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 5.0f;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}

vec3 getNormal() {
  vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

void main() {

  vec3 normal = getNormal();

  gs_out.texCoords = gs_in[0].texCoords;
  gl_Position = explode(gl_in[0].gl_Position, normal);
  EmitVertex();

  gs_out.texCoords = gs_in[1].texCoords;
  gl_Position = explode(gl_in[1].gl_Position, normal);
  EmitVertex();

  gs_out.texCoords = gs_in[2].texCoords;
  gl_Position = explode(gl_in[2].gl_Position, normal);
  EmitVertex();
  
  EndPrimitive();
}