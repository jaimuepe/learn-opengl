#version 450 core

in VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
}
fs_in;

out vec4 FragColor;

layout(std140, binding = 1) uniform Light {
  vec3 lightPos; // padded to vec4
};

uniform sampler2D diffuse_texture0;

void main() {

  vec3 color = vec3(texture(diffuse_texture0, fs_in.texCoords));
  vec3 n = normalize(fs_in.normal);

  // ambient
  vec3 ambient = 0.05 * color;

  // diffuse

  vec3 fragLightDir = normalize(lightPos - fs_in.fragPos);

  float diff = max(0.0, dot(fragLightDir, n));
  vec3 diffuse = diff * color;

  // specular

  vec3 fragCameraDir =
      normalize(-fs_in.fragPos); // cameraPos = 0.0 in camera space

  float spec;

  if (gl_FragCoord.x > 600) {

    // blinn-phong

    vec3 halfwayDir = normalize(fragLightDir + fragCameraDir);
    spec = pow(max(0.0, dot(halfwayDir, n)), 32.0);

  } else {

    // phong

    vec3 ref = reflect(-fragLightDir, n);
    spec = pow(max(0.0, dot(fragCameraDir, ref)), 8.0);
  }

  vec3 specular = vec3(0.3) * spec;

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}