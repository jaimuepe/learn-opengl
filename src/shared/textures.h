
#ifndef TEXTURES_H
#define TEXTURES_H

#include "filesystem.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <sstream>

namespace gpu {

namespace resources {

namespace texture2d {

enum class Wrap {
  REPEAT = 1,
  MIRRORED_REPEAT = 2,
  CLAMP_TO_EDGE = 4,
  CLAMP_TO_BORDER = 8
};

enum class Filter {
  NEAREST = 1,
  LINEAR = 2,
  NEAREST_MIPMAP_NEAREST = 4,
  LINEAR_MIPMAP_NEAREST = 8,
  NEAREST_MIPMAP_LINEAR = 16,
  LINEAR_MIPMAP_LINEAR = 32
};

namespace {

GLuint mapToGLWrap(Wrap wrap, unsigned int texId) {
  switch (wrap) {
  case Wrap::REPEAT:
    return GL_REPEAT;
  case Wrap::MIRRORED_REPEAT:
    return GL_MIRRORED_REPEAT;
  case Wrap::CLAMP_TO_EDGE:
    return GL_CLAMP_TO_EDGE;
  case Wrap::CLAMP_TO_BORDER:
    return GL_CLAMP_TO_BORDER;
  default:

    std::stringstream ss;
    ss << "Unrecognized wrap mode: " << static_cast<int>(wrap);
    std::string message = ss.str();

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                         texId, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                         message.c_str());
    return 0;
  }
}

GLuint mapToGLFilter(Filter filter, unsigned int texId) {
  switch (filter) {
  case Filter::NEAREST:
    return GL_NEAREST;
  case Filter::LINEAR:
    return GL_LINEAR;
  case Filter::NEAREST_MIPMAP_NEAREST:
    return GL_NEAREST_MIPMAP_NEAREST;
  case Filter::LINEAR_MIPMAP_NEAREST:
    return GL_LINEAR_MIPMAP_NEAREST;
  case Filter::NEAREST_MIPMAP_LINEAR:
    return GL_NEAREST_MIPMAP_LINEAR;
  case Filter::LINEAR_MIPMAP_LINEAR:
    return GL_LINEAR_MIPMAP_LINEAR;
  default:

    std::stringstream ss;
    ss << "Unrecognized filter mode: " << static_cast<int>(filter);
    std::string message = ss.str();

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                         texId, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                         message.c_str());
    return 0;
  }
}

} // namespace

struct CreateInfo {

  Wrap wrapS;
  Wrap wrapT;

  Filter minFilter;
  Filter magFilter;

  bool generateMipMaps;

  CreateInfo()
      : wrapS(Wrap::REPEAT), wrapT(Wrap::REPEAT), minFilter(Filter::LINEAR),
        magFilter(Filter::LINEAR), generateMipMaps(false) {}
};

unsigned int create(const std::string texName,
                    const CreateInfo &createInfo = {}) {

  unsigned int texId;

  stbi_set_flip_vertically_on_load(true);

  glCreateTextures(GL_TEXTURE_2D, 1, &texId);

  int width;
  int height;
  int nrChannels;

  std::string texPath = getTexturePath(texName);

  unsigned char *data =
      stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

  if (data) {

    GLenum format;
    GLenum internalFormat;

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

      std::stringstream ss;
      ss << "Unexpected # of channels in texture (" << nrChannels
         << " channels)" << std::endl;

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           texId, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());

      stbi_image_free(data);
      return texId;
    }

    glTextureParameteri(texId, GL_TEXTURE_WRAP_S,
                        mapToGLWrap(createInfo.wrapS, texId));
    glTextureParameteri(texId, GL_TEXTURE_WRAP_T,
                        mapToGLWrap(createInfo.wrapT, texId));

    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER,
                        mapToGLFilter(createInfo.minFilter, texId));
    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER,
                        mapToGLFilter(createInfo.magFilter, texId));

    glTextureStorage2D(texId, 1, internalFormat, width, height);
    glTextureSubImage2D(texId, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE,
                        data);

    if (createInfo.generateMipMaps) {
      glGenerateTextureMipmap(texId);
    }

    stbi_image_free(data);

    return texId;
  } else {

    std::stringstream ss;
    ss << "Could not create texture " << texPath;

    std::string message = ss.str();

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                         texId, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                         message.c_str());

    return 0;
  }
}

}; // namespace texture2d
}; // namespace resources
}; // namespace gpu

#endif // TEXTURES_H