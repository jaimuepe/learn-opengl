
#ifndef GPU_CONSTANTS_H
#define GPU_CONSTANTS_H

#include "bitmask.h"

#include <glad/glad.h>

#include <sstream>
#include <string>

namespace gpu {

namespace framebuffer {

enum class ClearFlagBits : unsigned {
  COLOR_BIT = GL_COLOR_BUFFER_BIT,
  DEPTH_BIT = GL_DEPTH_BUFFER_BIT,
  STENCIL_BIT = GL_STENCIL_BUFFER_BIT
};

ENABLE_BITMASK_OPERATORS(ClearFlagBits)

} // namespace framebuffer

namespace texture {

enum class Wrap : unsigned {
  REPEAT = GL_REPEAT,
  MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
  CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
  CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
};

enum class Filter : unsigned {
  NEAREST = GL_NEAREST,
  LINEAR = GL_LINEAR,
  NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
  LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
  NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
  LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

} // namespace texture

} // namespace gpu

#endif // GPU_CONSTANTS_H