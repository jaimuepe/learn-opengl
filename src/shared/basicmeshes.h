#ifndef BASIC_MESHES_H
#define BASIC_MESHES_H

#include <glm/glm.hpp>

#include <vector>

struct MeshCreateInfo {

  MeshCreateInfo()
      : scale(glm::vec3{1.0f}), insideOut(false), uvScale(glm::vec2{1.0f}) {}

  glm::vec3 scale;
  bool insideOut;
  glm::vec2 uvScale;
};

namespace {

void manipulateData(std::vector<float> &data, const glm::vec3 &scale,
                    bool insideOut, const glm::vec2 &uvScale) {

  size_t nRows = data.size() / 8;

  for (size_t i = 0; i < nRows; ++i) {

    int index = 8 * i;

    data[index] *= scale.x;
    data[index + 1] *= scale.y;
    data[index + 2] *= scale.z;

    data[index + 6] *= uvScale.s;
    data[index + 7] *= uvScale.t;

    if (insideOut) {

      if (i > 0 && (i + 1) % 3 == 0) {

        // each 3 triangles we swap the second and the third

        float x = data[index - 8];
        float y = data[index - 8 + 1];
        float z = data[index - 8 + 2];

        float s = data[index - 8 + 6];
        float t = data[index - 8 + 7];

        data[index - 8] = data[index];
        data[index] = x;

        data[index - 8 + 1] = data[index + 1];
        data[index + 1] = y;

        data[index - 8 + 2] = data[index + 2];
        data[index + 2] = z;

        data[index - 8 + 6] = data[index + 6];
        data[index + 6] = s;

        data[index - 8 + 7] = data[index + 7];
        data[index + 7] = t;
      }

      data[index + 3] *= -1;
      data[index + 4] *= -1;
      data[index + 5] *= -1;
    }
  }
}

} // namespace

std::vector<float> createCubeVertexData(const MeshCreateInfo &createInfo = {}) {

  // clang-format off

  std::vector<float> vertexData = {
    
    // position             // normals            // uvs
    
    // back face
     0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f, // top-left
     0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f, // top-left
    -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f, // top-right

    // front face
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f, // top-right

    // left face
    -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
    -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, // top-right

    // right face
     0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
     0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f, // top-right

    // bottom face
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f, // top-left
     0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
     0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f, // top-right

    // top-face
    -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f, // bottom-left
     0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f, // top-left
    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f, // top-right
  };

  // clang-format on

  manipulateData(vertexData, createInfo.scale, createInfo.insideOut,
                 createInfo.uvScale);

  return std::move(vertexData);
}

std::vector<float> createQuadVertexData(const MeshCreateInfo &createInfo = {}) {

  // defaults to plane XZ

  // clang-format off
  std::vector<float> vertexData = {

    // position          // normals          // uvs
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top-left
    -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom left
     1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right

     1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top-right
    -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom-left
     1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f  // bottom-right
  };

  manipulateData(vertexData, createInfo.scale, createInfo.insideOut, createInfo.uvScale);
  return std::move(vertexData);

  // clang-format on
}

#endif // BASIC_MESHES_H