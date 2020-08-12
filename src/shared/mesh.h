#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include "texture2d.h"
#include "vertex.h"

#include <glad/glad.h>

#include <optional>
#include <vector>

struct MeshTexture {
  std::string type;
  gpu::texture::Texture2D texture;
};

struct MeshData {
  std::vector<Vertex> vertices;
  std::optional<std::vector<unsigned int>> indices;
  std::vector<MeshTexture> textures;
};

class Mesh {

public:
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  std::vector<MeshTexture> m_textures;

  Mesh() {}

  Mesh(const MeshData &data)
      : m_vertices(data.vertices), m_textures(data.textures) {

    m_indexed = data.indices.has_value();
    if (m_indexed) {
      m_indices = data.indices.value();
    }

    setupMesh();
  }

  inline GLuint getVAO() const { return m_VAO; }

  void draw(const gpu::Shader &shader) const {

    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;

    // prepare textures

    for (unsigned int i = 0; i < m_textures.size(); ++i) {

      glBindTextureUnit(i, m_textures[i].texture.getID());

      std::string name = m_textures[i].type;
      std::string number;

      if (name == "texture_diffuse") {
        number = std::to_string(diffuseNr++);
      } else if (name == "texture_specular") {
        number = std::to_string(specularNr++);
      }

      shader.setInt("material." + name + number, i);
    }

    // draw mesh

    glBindVertexArray(m_VAO);

    if (m_indexed) {
      glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }

    glBindVertexArray(0);
  }

private:
  bool m_indexed;

  unsigned int m_VAO;
  unsigned int m_VBO;
  unsigned int m_EBO;

  void setupMesh() {

    glCreateBuffers(1, &m_VBO);
    glNamedBufferData(m_VBO, m_vertices.size() * sizeof(Vertex), &m_vertices[0],
                      GL_STATIC_DRAW);

    glCreateVertexArrays(1, &m_VAO);

    // indices

    if (m_indexed) {
      glCreateBuffers(1, &m_EBO);
      glNamedBufferData(m_EBO, m_indices.size() * sizeof(unsigned int),
                        &m_indices[0], GL_STATIC_DRAW);

      glVertexArrayElementBuffer(m_VAO, m_EBO);
    }

    // positions
    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(Vertex));
    glVertexArrayAttribBinding(m_VAO, 0, 0);

    // normals
    glEnableVertexArrayAttrib(m_VAO, 1);
    glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(m_VAO, 1, m_VBO, offsetof(Vertex, normal),
                              sizeof(Vertex));
    glVertexArrayAttribBinding(m_VAO, 1, 1);

    // texcoords
    glEnableVertexArrayAttrib(m_VAO, 2);
    glVertexArrayAttribFormat(m_VAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(m_VAO, 2, m_VBO, offsetof(Vertex, texCoords),
                              sizeof(Vertex));
    glVertexArrayAttribBinding(m_VAO, 2, 2);

    // tangent
    glEnableVertexArrayAttrib(m_VAO, 3);
    glVertexArrayAttribFormat(m_VAO, 3, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(m_VAO, 3, m_VBO, offsetof(Vertex, tangent),
                              sizeof(Vertex));
    glVertexArrayAttribBinding(m_VAO, 3, 3);

    // bitangent
    glEnableVertexArrayAttrib(m_VAO, 4);
    glVertexArrayAttribFormat(m_VAO, 4, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(m_VAO, 4, m_VBO, offsetof(Vertex, bitangent),
                              sizeof(Vertex));
    glVertexArrayAttribBinding(m_VAO, 4, 4);

    glBindVertexArray(0);
  }
};

#endif // MESH_H