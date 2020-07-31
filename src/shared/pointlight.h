
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/glm.hpp>

struct PointLight {

  glm::vec3 position;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;

  float constantAtt;
  float linearAtt;
  float quadraticAtt;
};

#endif // POINT_LIGHT_H