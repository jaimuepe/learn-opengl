
#ifndef GPU_OBJECT_H
#define GPU_OBJECT_H

namespace gpu {

#define GPU_OBJECT_CREATE_LAZY(glFunction)                                     \
  if (m_ID == 0) {                                                             \
    glFunction(1, &m_ID);                                                      \
  }

class GpuObject {

public:
  GpuObject() : m_ID(0) {}

  inline unsigned int getID() const { return m_ID; }

  virtual void destroy() = 0;

protected:
  unsigned int m_ID;
};

} // namespace gpu

#endif // GPU_OBJECT_H