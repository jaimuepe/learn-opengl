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
      glTextureStorage2D(m_ID, 1, texData->internalFormat, texData->width,
                         texData->height);
      glTextureSubImage2D(m_ID, 0, 0, 0, texData->width, texData->height,
                          texData->format, GL_UNSIGNED_BYTE, texData->rawData);
    }
  }

  Texture2D(int width, int height, unsigned int internalFormat) {
    glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
    glTextureStorage2D(m_ID, 1, internalFormat, width, height);
  }
};

} // namespace texture

} // namespace gpu

#endif // GPU_TEXTURE_2D_H