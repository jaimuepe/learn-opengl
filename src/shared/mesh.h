#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include "vertex.h"

#include <glad/glad.h>

#include <vector>

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {

public:
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  std::vector<Texture> m_textures;

  Mesh(const std::vector<Vertex> &vertices,
       const std::vector<unsigned int> &indices,
       const std::vector<Texture> &textures)
      : m_vertices(vertices), m_indices(indices), m_textures(textures) {
    setupMesh();
  }

  inline GLuint getVAO() const { return m_VAO; }

  void draw(const Shader &shader) const {

    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;

    // prepare textures

    for (unsigned int i = 0; i < m_textures.size(); ++i) {

      glBindTextureUnit(i, m_textures[i].id);

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
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

private:
  unsigned int m_VAO;
  unsigned int m_VBO;
  unsigned int m_EBO;

  void setupMesh() {

    glCreateBuffers(1, &m_VBO);
    glNamedBufferData(m_VBO, m_vertices.size() * sizeof(Vertex), &m_vertices[0],
                      GL_STATIC_DRAW);

    glCreateBuffers(1, &m_EBO);
    glNamedBufferData(m_EBO, m_indices.size() * sizeof(unsigned int),
                      &m_indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &m_VAO);

    // indices
    glVertexArrayElementBuffer(m_VAO, m_EBO);

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

    glBindVertexArray(0);
  }
};

#endif // MESH_H