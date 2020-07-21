#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 invModel;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 viewPos;

out vec3 Color;

void main() {

  // ambient light
  float ambientStrength = 0.2;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse light

  vec3 n = normalize(mat3(transpose(invModel)) * aNormal);

  vec3 fragPos = vec3(model * vec4(aPos, 1.0));

  // direction fragment->light
  vec3 lightDir = normalize(lightPos - fragPos);

  float diff = max(0.0, dot(n, lightDir));
  vec3 diffuse = diff * lightColor;

  // specular light

  float specularStrength = 0.5;

  // negative! we want light->fragment now to get fragment->light'
  vec3 reflectDir = reflect(-lightDir, n);

  // fragment->eye
  vec3 viewDir = normalize(viewPos - fragPos);

  float spec = pow(max(0.0, dot(reflectDir, viewDir)), 32);

  vec3 specular = spec * lightColor;

  Color = ambient + diffuse + specular;

  gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}