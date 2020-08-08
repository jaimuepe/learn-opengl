
#ifndef GPU_VERTEX_ARRAY_H
#define GPU_VERTEX_ARRAY_H

#include "gpuobject.h"
#include "vertexbuffer.h"

#include <glad/glad.h>

namespace gpu {

struct VertexArrayBufferBindInfo {

  VertexBuffer *pVertexBuffer;

  size_t nBindings;

  size_t *pBindIndices;
  size_t *pValuesPerVertex;
};

class VertexArray : public GpuObject {

public:
  VertexArray() { glCreateVertexArrays(1, &m_ID); }

  void bindBuffer(const VertexArrayBufferBindInfo &bindInfo) {

    GLuint vertexBufferID = bindInfo.pVertexBuffer->getID();

    size_t stride = 0;
    for (size_t i = 0; i < bindInfo.nBindings; ++i) {
      stride += bindInfo.pValuesPerVertex[i];
    }

    size_t offset = 0;

    for (size_t i = 0; i < bindInfo.nBindings; ++i) {

      size_t index = bindInfo.pBindIndices[i];
      size_t valuesPerVertex = bindInfo.pValuesPerVertex[i];

      glEnableVertexArrayAttrib(m_ID, index);

      glVertexArrayAttribFormat(m_ID, index, valuesPerVertex, GL_FLOAT,
                                GL_FALSE, 0);

      glVertexArrayVertexBuffer(m_ID, index, vertexBufferID,
                                offset * sizeof(float), stride * sizeof(float));

      glVertexArrayAttribBinding(m_ID, index, index);

      offset += valuesPerVertex;
    }
  }

  inline void bind() const { glBindVertexArray(m_ID); }

  virtual void destroy() override {
    glDeleteVertexArrays(1, &m_ID);
    m_ID = 0;
  }
};

} // namespace gpu

#endif // GPU_VERTEX_ARRAY_H