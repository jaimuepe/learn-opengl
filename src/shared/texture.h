#ifndef GPU_TEXTURE_H
#define GPU_TEXTURE_H

#include "gpuconstants.h"
#include "gpuobject.h"
#include "resources.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <memory>
#include <string>

namespace gpu {

namespace texture {

struct TextureData {

  unsigned char *rawData;

  int width;
  int height;

  GLuint format;
  GLuint internalFormat;

  operator bool() const { return static_cast<bool>(rawData); }

  ~TextureData() {
    if (rawData) {
      stbi_image_free(rawData);
    }
  }
};

typedef std::unique_ptr<TextureData> UniqueTextureData;

class Texture : public GpuObject {

public:
  inline void setWrapS(Wrap wrapMode) {
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S,
                        static_cast<unsigned int>(wrapMode));
  }

  inline void setWrapT(Wrap wrapMode) {
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T,
                        static_cast<unsigned int>(wrapMode));
  }

  inline void setWrapST(Wrap wrap) {
    setWrapS(wrap);
    setWrapT(wrap);
  }

  inline void setMinFilter(Filter filter) {
    glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER,
                        static_cast<unsigned int>(filter));
  }

  inline void setMagFilter(Filter filter) {
    glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER,
                        static_cast<unsigned int>(filter));
  }

  inline void setMinMagFilter(Filter filter) {
    setMinFilter(filter);
    setMagFilter(filter);
  }

  inline void setBorderColor(const glm::vec4 &borderColor) {
    glTextureParameterfv(m_ID, GL_TEXTURE_BORDER_COLOR,
                         glm::value_ptr(borderColor));
  }

  inline void generateMipmap() { glGenerateTextureMipmap(m_ID); }

  virtual void destroy() override {
    glDeleteTextures(1, &m_ID);
    m_ID = 0;
  }

protected:
  Texture() : m_width(0), m_height(0), m_format(0), m_internalFormat(0) {}

  UniqueTextureData loadTexture(const std::string &path, bool flipY) {

    stbi_set_flip_vertically_on_load(flipY);

    int nrChannels;

    UniqueTextureData texData = std::make_unique<TextureData>();

    unsigned char *data = stbi_load(path.c_str(), &texData->width,
                                    &texData->height, &nrChannels, 0);

    if (data) {

      texData->rawData = data;

      if (nrChannels == 1) {
        texData->format = GL_RED;
        texData->internalFormat = GL_R8;
      } else if (nrChannels == 3) {
        texData->format = GL_RGB;
        texData->internalFormat = GL_RGB8;
      } else if (nrChannels == 4) {
        texData->format = GL_RGBA;
        texData->internalFormat = GL_RGBA8;
      } else {

        std::stringstream ss;
        ss << "Unexpected # of channels in texture (" << nrChannels
           << " channels)" << std::endl;

        std::string message = ss.str();

        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                             m_ID, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                             message.c_str());
      }
    } else {

      std::stringstream ss;
      ss << "Could not load texture data from : " << path;

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           m_ID, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());
    }

    return texData;
  }

  size_t m_width;
  size_t m_height;
  GLuint m_format;
  GLuint m_internalFormat;
};

GLuint internalFormatFromFormat(GLuint format) {

  switch (format) {
  case GL_RGB:
    return GL_RGB8;
  case GL_RGBA:
    return GL_RGBA8;
  case GL_R:
    return GL_R8;
  default:

    std::stringstream ss;
    ss << "texture::internalFormatFromFormat\n";
    ss << "Unhandled format: " << format;

    std::string message = ss.str();

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
                         GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                         message.c_str());

    return 0;
  }
}

} // namespace texture

} // namespace gpu

#endif // GPU_TEXTURE_H