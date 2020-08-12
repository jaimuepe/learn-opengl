#ifndef GPU_CUBEMAP_H
#define GPU_CUBEMAP_H

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

class Cubemap : public Texture {

public:
  Cubemap() {}

  Cubemap(const std::string &name, const std::string &posXFace,
          const std::string &negXFace, const std::string &posYFace,
          const std::string &negYFace, const std::string &posZFace,
          const std::string &negZFace) {

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_ID);

    std::string faces[] = {posXFace, negXFace, posYFace,
                           negYFace, posZFace, negZFace};

    for (unsigned int i = 0; i < 6; ++i) {

      std::string path = getSkyboxPath(name, faces[i]);

      UniqueTextureData faceData = loadTexture(path, false);

      if (faceData) {

        if (i == 0) {

          m_width = faceData->width;
          m_height = faceData->height;
          m_format = faceData->format;
          m_internalFormat = faceData->internalFormat;

          glTextureStorage2D(m_ID, 1, m_internalFormat, m_width, m_height);
        }

        glTextureSubImage3D(m_ID, 0, 0, 0, i, faceData->width, faceData->height,
                            1, faceData->format, GL_UNSIGNED_BYTE,
                            faceData->rawData);
      }
    }

    setMinMagFilter(Filter::LINEAR);
    setWrapRST(Wrap::CLAMP_TO_EDGE);
  }

  Cubemap(int width, int height, unsigned int internalFormat) {

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_ID);
    glTextureStorage2D(m_ID, 1, internalFormat, width, height);
  }

  inline void setWrapR(Wrap wrapMode) {
    glTextureParameteri(m_ID, GL_TEXTURE_WRAP_R,
                        static_cast<unsigned int>(wrapMode));
  }

  inline void setWrapRST(Wrap wrap) {
    setWrapS(wrap);
    setWrapT(wrap);
    setWrapR(wrap);
  }
};

} // namespace texture
} // namespace gpu

#endif // GPU_CUBEMAP_H