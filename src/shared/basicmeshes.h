#ifndef BASIC_MESHES_H
#define BASIC_MESHES_H

#include "mesh.h"

#include <glm/glm.hpp>

#include <vector>

struct MeshCreateInfo {

  MeshCreateInfo() {}

  glm::vec3 scale = glm::vec3{1.0f};
  bool insideOut = false;
  glm::vec2 uvScale = glm::vec2{1.0f};
};

namespace {

MeshData createMeshData(const std::vector<float> &vertexData) {

  // 3x position, 2x texcoords
  assert(vertexData.size() % 15 == 0);

  std::vector<Vertex> vertices;

  unsigned int nTris = vertexData.size() / 15;

  // +15 for next triangle
  for (size_t i = 0; i < nTris; ++i) {

    for (size_t j = 0; j < 3; ++j) {

      float x = vertexData[(3 * i + j) * 5];
      float y = vertexData[(3 * i + j) * 5 + 1];
      float z = vertexData[(3 * i + j) * 5 + 2];

      float s = vertexData[(3 * i + j) * 5 + 3];
      float t = vertexData[(3 * i + j) * 5 + 4];

      Vertex vert;
      vert.position = glm::vec3{x, y, z};
      vert.texCoords = glm::vec2{s, t};

      vertices.push_back(vert);
    }

    // end of triangle

    Vertex &v0 = vertices[3 * i];
    Vertex &v1 = vertices[3 * i + 1];
    Vertex &v2 = vertices[3 * i + 2];

    glm::vec3 &a = v0.position;
    glm::vec3 &b = v1.position;
    glm::vec3 &c = v2.position;

    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - a;

    glm::vec3 normal = glm::normalize(glm::cross(e1, e2));

    v0.normal = normal;
    v1.normal = normal;
    v2.normal = normal;

    glm::vec2 &uvA = v0.texCoords;
    glm::vec2 &uvB = v1.texCoords;
    glm::vec2 &uvC = v2.texCoords;

    glm::vec2 dUV1 = uvB - uvA;
    glm::vec2 dUV2 = uvC - uvA;

    float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

    glm::vec3 tangent;
    tangent.x = f * (dUV2.y * e1.x - dUV1.y * e2.x);
    tangent.y = f * (dUV2.y * e1.y - dUV1.y * e2.y);
    tangent.z = f * (dUV2.y * e1.z - dUV1.y * e2.z);

    glm::vec3 bitangent;
    bitangent.x = f * (-dUV2.x * e1.x + dUV1.x * e2.x);
    bitangent.y = f * (-dUV2.x * e1.y + dUV1.x * e2.y);
    bitangent.z = f * (-dUV2.x * e1.z + dUV1.x * e2.z);

    v0.tangent = tangent;
    v1.tangent = tangent;
    v2.tangent = tangent;

    v0.bitangent = bitangent;
    v1.bitangent = bitangent;
    v2.bitangent = bitangent;
  }

  MeshData meshData;
  meshData.vertices = vertices;

  return meshData;
}

void manipulateData(MeshData &meshData, const MeshCreateInfo &createInfo) {

  std::vector<Vertex> &vertices = meshData.vertices;

  assert(vertices.size() % 3 == 0);

  for (size_t i = 0; i < vertices.size(); i += 3) {

    Vertex &v0 = vertices[i];
    Vertex &v1 = vertices[i + 1];
    Vertex &v2 = vertices[i + 2];

    v0.position *= createInfo.scale;
    v1.position *= createInfo.scale;
    v2.position *= createInfo.scale;

    v0.texCoords *= createInfo.uvScale;

    if (createInfo.insideOut) {

      // swap 2nd and 3rd vertices
      std::swap(vertices[i + 1], vertices[i + 2]);

      // invert normals
      v0.normal *= -1.0f;
      v1.normal *= -1.0f;
      v2.normal *= -1.0f;
    }
  }
}

} // namespace

Mesh createQuad(const MeshCreateInfo &createInfo = {}) {

  // clang-format off

  std::vector<float> vertexData = {
    // position           // uvs
    -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, // top-left
    -1.0f, 0.0f,  1.0f,   0.0f, 0.0f, // bottom left
     1.0f, 0.0f, -1.0f,   1.0f, 1.0f, // top-right

     1.0f, 0.0f, -1.0f,   1.0f, 1.0f, // top-right
    -1.0f, 0.0f,  1.0f,   0.0f, 0.0f, // bottom-left
     1.0f, 0.0f,  1.0f,   1.0f, 0.0f  // bottom-right
  };
  // clang-format on

  MeshData meshData = createMeshData(vertexData);
  manipulateData(meshData, createInfo);

  return Mesh{meshData};
}

Mesh createCube(const MeshCreateInfo &createInfo = {}) {

  // clang-format off

  std::vector<float> vertexData = {
    
    // position             // uvs
    
    // back face
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // top-left
     0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // bottom-right

     0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // top-left
    -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right

    // front face
    -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // top-left

    -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // top-right

    // left face
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,    1.0f, 1.0f, // top-right

    // right face
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // top-left

     0.5f,  0.5f,  0.5f,    0.0f, 1.0f, // top-left
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right

    // bottom face
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, // bottom-left
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // bottom-right
    -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, // top-left

    -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, // top-left
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f, // bottom-right
     0.5f, -0.5f,  0.5f,    1.0f, 1.0f, // top-right

    // top-face
    -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, // bottom-left
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left

    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f, // top-right
  };

  MeshData meshData = createMeshData(vertexData);
  manipulateData(meshData, createInfo);

  return Mesh{meshData};
}

#endif // BASIC_MESHES_H