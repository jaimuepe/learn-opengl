#ifndef GPU_TEXTURE_2D_H
#define GPU_TEXTURE_2D_H

#include "gpuconstants.h"
#include "resources.h"
#include "texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <string>

namespace gpu {

namespace texture {

class Texture2D : public Texture {

public:
  Texture2D() {}

  Texture2D(const std::string &textureName) {

    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

    UniqueTextureData texData = loadTexture(getTexturePath(textureName), true);

    if (texData) {

      m_width = texData->width;
      m_height = texData->height;
      m_internalFormat = texData->internalFormat;
      m_format = texData->format;

      glTextureStorage2D(m_ID, 1, m_internalFormat, m_width, m_height);
      glTextureSubImage2D(m_ID, 0, 0, 0, m_width, m_height, m_format,
                          GL_UNSIGNED_BYTE, texData->rawData);
    }
  }

  Texture2D(int width, int height, unsigned int format) {

    m_width = width;
    m_height = height;
    m_format = format;
    m_internalFormat = internalFormatFromFormat(format);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
    glTextureStorage2D(m_ID, 1, m_internalFormat, width, height);
  }

  void subImage(size_t x, size_t y, size_t width, size_t height,
                const float *data) {
    glTextureSubImage2D(m_ID, 0, x, y, width, height, m_format, GL_FLOAT, data);
  }
};

Texture2D createUnitTexture2D(const glm::vec3 &color) {

  Texture2D tex{1, 1, GL_RGB};

  tex.setWrapST(Wrap::CLAMP_TO_EDGE);
  tex.setMinMagFilter(Filter::NEAREST);

  tex.subImage(0, 0, 1, 1, glm::value_ptr(color));

  return tex;
}

} // namespace texture

} // namespace gpu

#endif // GPU_TEXTURE_2D_H