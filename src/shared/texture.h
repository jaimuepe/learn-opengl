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
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, mapToGLWrap(wrapMode, m_ID));
  }

  inline void setWrapT(Wrap wrapMode) {
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, mapToGLWrap(wrapMode, m_ID));
  }

  inline void setWrapST(Wrap wrap) {
    setWrapS(wrap);
    setWrapT(wrap);
  }

  inline void setMinFilter(Filter filter) {
    glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER,
                        mapToGLFilter(filter, m_ID));
  }

  inline void setMagFilter(Filter filter) {
    glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER,
                        mapToGLFilter(filter, m_ID));
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
  Texture() {}

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
};

} // namespace texture

} // namespace gpu

#endif // GPU_TEXTURE_H