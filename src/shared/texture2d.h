#ifndef GPU_TEXTURE_H
#define GPU_TEXTURE_H

#include "gpuconstants.h"
#include "gpuobject.h"
#include "resources.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <string>

namespace gpu {

class Texture2D : public GpuObject {

public:
  Texture2D() {}

  Texture2D(const std::string &texturePath) {

    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

    int width;
    int height;
    int nrChannels;

    std::string texPath = getTexturePath(texturePath);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data =
        stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

    if (data) {

      GLuint format;
      GLuint internalFormat;

      if (nrChannels == 1) {
        format = GL_RED;
        internalFormat = GL_R8;
      } else if (nrChannels == 3) {
        format = GL_RGB;
        internalFormat = GL_RGB8;
      } else if (nrChannels == 4) {
        format = GL_RGBA;
        internalFormat = GL_RGBA8;
      } else {

        format = 0;
        internalFormat = 0;

        std::stringstream ss;
        ss << "Unexpected # of channels in texture (" << nrChannels
           << " channels)" << std::endl;

        std::string message = ss.str();

        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                             m_ID, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                             message.c_str());

        stbi_image_free(data);
      }

      glTextureStorage2D(m_ID, 1, internalFormat, width, height);
      glTextureSubImage2D(m_ID, 0, 0, 0, width, height, format,
                          GL_UNSIGNED_BYTE, data);
    } else {

      std::stringstream ss;
      ss << "Could not load texture data from : " << texPath;

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           m_ID, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());
    }
  }

  Texture2D(int width, int height, unsigned int internalFormat) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
    glTextureStorage2D(m_ID, 1, internalFormat, width, height);
  }

  inline void setWrapS(gpu::texture::Wrap wrapMode) {
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, mapToGLWrap(wrapMode, m_ID));
  }

  inline void setWrapT(gpu::texture::Wrap wrapMode) {
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, mapToGLWrap(wrapMode, m_ID));
  }

  inline void setWrapST(gpu::texture::Wrap wrap) {
    setWrapS(wrap);
    setWrapT(wrap);
  }

  inline void setWrapST(gpu::texture::Wrap wrapS, gpu::texture::Wrap wrapT) {
    setWrapS(wrapS);
    setWrapT(wrapT);
  }

  inline void setMinFilter(gpu::texture::Filter filter) {
    glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER,
                        mapToGLFilter(filter, m_ID));
  }

  inline void setMagFilter(gpu::texture::Filter filter) {
    glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER,
                        mapToGLFilter(filter, m_ID));
  }

  inline void setMinMagFilter(gpu::texture::Filter filter) {
    setMinFilter(filter);
    setMagFilter(filter);
  }

  inline void setMinMagFilter(gpu::texture::Filter minFilter,
                              gpu::texture::Filter magFilter) {
    setMinFilter(minFilter);
    setMagFilter(magFilter);
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
};

} // namespace gpu

#endif // GPU_TEXTURE_H