
#ifndef GPU_RENDERBUFFER_H
#define GPU_RENDERBUFFER_H

#include "gpuobject.h"

#include <glad/glad.h>

namespace gpu {

class Renderbuffer : public GpuObject {

public:
  Renderbuffer(int width, int height, unsigned int internalFormat) {
    glCreateRenderbuffers(1, &m_ID);
    glNamedRenderbufferStorage(m_ID, internalFormat, width, height);
  }

  virtual void destroy() override {
    glDeleteRenderbuffers(1, &m_ID);
    m_ID = 0;
  }
};

} // namespace gpu

#endif // GPU_RENDERBUFFER_H