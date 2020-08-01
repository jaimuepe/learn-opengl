#version 450 core

in VS_OUT { 
  vec2 texCoords;
} fs_in;

out vec4 FragColor;

struct Material {
  sampler2D texture_diffuse0;
};

uniform Material material;

void main() { FragColor = texture(material.texture_diffuse0, fs_in.texCoords); }