#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "resources.h"
#include "shader.h"
#include "texture2d.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/common.hpp>
#include <stb_image.h>

#include <filesystem>
#include <map>
#include <sstream>
#include <vector>

std::map<std::string, MeshTexture> textures_loaded;

gpu::texture::Texture2D textureFromFile(const std::string &path,
                                        const std::string &directory);

class Model
{

public:
  std::vector<Mesh> m_meshes;

  Model() {}

  Model(const std::string &path) { loadModel(path); }

  void draw(const gpu::Shader &shader)
  {
    for (unsigned int i = 0; i < m_meshes.size(); ++i)
    {
      m_meshes[i].draw(shader);
    }
  }

private:
  std::string m_directory;

  void loadModel(const std::string &path)
  {

    Assimp::Importer importer;

    const aiScene *scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                    aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode)
    {

      std::stringstream ss;
      ss << "Error loading model " << path << ":\n"
         << importer.GetErrorString() << std::endl;

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
                           GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());

      return;
    }

    std::filesystem::path dirPath(path);
    m_directory = dirPath.parent_path().string();

    processNode(scene->mRootNode, scene);
  }

  void processNode(aiNode *node, const aiScene *scene)
  {

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      m_meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
      processNode(node->mChildren[i], scene);
    }
  }

  Mesh processMesh(aiMesh *mesh, const aiScene *scene) const
  {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    // process vertex positions
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
      Vertex vertex;

      glm::vec3 position;
      position.x = mesh->mVertices[i].x;
      position.y = mesh->mVertices[i].y;
      position.z = mesh->mVertices[i].z;
      vertex.position = position;

      glm::vec3 normal;
      normal.x = mesh->mNormals[i].x;
      normal.y = mesh->mNormals[i].y;
      normal.z = mesh->mNormals[i].z;
      vertex.normal = normal;

      if (mesh->mTextureCoords[0])
      {
        glm::vec2 texCoords;
        texCoords.x = mesh->mTextureCoords[0][i].x;
        texCoords.y = mesh->mTextureCoords[0][i].y;
        vertex.texCoords = texCoords;
      }

      glm::vec3 tangent;
      tangent.x = mesh->mTangents[i].x;
      tangent.y = mesh->mTangents[i].y;
      tangent.z = mesh->mTangents[i].z;
      vertex.tangent = tangent;

      glm::vec3 bitangent;
      bitangent.x = mesh->mBitangents[i].x;
      bitangent.y = mesh->mBitangents[i].y;
      bitangent.z = mesh->mBitangents[i].z;
      vertex.bitangent = tangent;

      vertices.push_back(vertex);
    }

    // process indices

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; ++j)
      {
        indices.push_back(face.mIndices[j]);
      }
    }

    // process material

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");

    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<MeshTexture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");

    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    MeshData meshData;
    meshData.vertices = vertices;
    meshData.indices = indices;
    meshData.textures = textures;

    return Mesh(meshData);
  }

  std::vector<MeshTexture> loadMaterialTextures(const aiMaterial *mat,
                                                aiTextureType type,
                                                std::string typeName) const
  {
    std::vector<MeshTexture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {

      aiString str;
      mat->GetTexture(type, i, &str);

      std::string texPath = str.C_Str();

      std::map<std::string, MeshTexture>::iterator it =
          textures_loaded.find(texPath);

      if (it == textures_loaded.end())
      {

        MeshTexture meshTexture;

        gpu::texture::Texture2D tex = textureFromFile(texPath, m_directory);
        meshTexture.texture = tex;
        meshTexture.type = typeName;

        textures_loaded[texPath] = meshTexture;

        textures.push_back(meshTexture);
      }
      else
      {
        textures.push_back(it->second);
      }
    }

    return textures;
  }
};

gpu::texture::Texture2D textureFromFile(const std::string &path,
                                        const std::string &directory)
{

  std::string filename = directory + separator + path;

  gpu::texture::Texture2D tex{path};
  tex.generateMipmap();
  tex.setMinFilter(gpu::texture::Filter::LINEAR_MIPMAP_LINEAR);

  return tex;
}

#endif // MODEL_H