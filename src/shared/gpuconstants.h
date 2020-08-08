
#ifndef GPU_CONSTANTS_H
#define GPU_CONSTANTS_H

#include <glad/glad.h>

#include <sstream>
#include <string>

namespace gpu {

namespace texture {

enum class Wrap { REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER };

enum class Filter {
  NEAREST,
  LINEAR,
  NEAREST_MIPMAP_NEAREST,
  LINEAR_MIPMAP_NEAREST,
  NEAREST_MIPMAP_LINEAR,
  LINEAR_MIPMAP_LINEAR
};

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

} // namespace texture

} // namespace gpu

#endif // GPU_CONSTANTS_H