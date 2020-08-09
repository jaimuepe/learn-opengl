
#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include "gpuobject.h"

#include <glad/glad.h>

namespace gpu {

class Buffer : public GpuObject {

public:
  virtual void destroy() override {
    glDeleteBuffers(1, &m_ID);
    m_ID = 0;
  }

protected:
  size_t m_size;

  Buffer() {}

  Buffer(size_t sizeBytes) : m_size(sizeBytes) {
    glCreateBuffers(1, &m_ID);
    glNamedBufferData(m_ID, m_size, nullptr, GL_STATIC_DRAW);
  }

  template <typename T>
  Buffer(const std::vector<T> &data) : m_size(sizeof(T) * data.size()) {
    glCreateBuffers(1, &m_ID);
    glNamedBufferData(m_ID, m_size, data.size() > 0 ? &data[0] : nullptr,
                      GL_STATIC_DRAW);
  }
};

} // namespace gpu

#endif // GPU_BUFFER_H