
#ifndef GPU_VERTEX_BUFFER_H
#define GPU_VERTEX_BUFFER_H

#include "buffer.h"
#include "gpuobject.h"

#include <glad/glad.h>

namespace gpu {

class VertexBuffer : public Buffer {

public:
  VertexBuffer() {}
  VertexBuffer(size_t sizeBytes) : Buffer(sizeBytes) {}

  template <typename T>
  VertexBuffer(const std::vector<T> &data) : Buffer(data) {}
};

} // namespace gpu

#endif // GPU_VERTEX_BUFFER_H