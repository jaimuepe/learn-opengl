#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include "buffer.h"

#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>

namespace gpu {

struct UniformBufferCreateInfo {

  size_t bindingIndex;

  size_t nBlocks;
  std::string *pBlockNames;
  size_t *pBlockSizes;
};

namespace {

struct Block {
  size_t offset;
  size_t size;
};

// Round the number to a multiple of 16
int round(int n) {

  int remainder = n % 16;

  if (remainder == 0) {
    return n;
  }

  return n + 16 - remainder;
}

size_t bufferSize(const UniformBufferCreateInfo &createInfo) {
  size_t size = 0;
  for (size_t i = 0; i < createInfo.nBlocks; ++i) {
    size += round(createInfo.pBlockSizes[i]);
  }
  return size;
}

} // namespace

class UniformBuffer : public Buffer {

public:
  UniformBuffer() {}

  UniformBuffer(size_t sizeBytes, size_t bindingIndex) : Buffer(m_size) {
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, m_ID);
  }

  UniformBuffer(const UniformBufferCreateInfo &createInfo)
      : Buffer(bufferSize(createInfo)) {

    size_t offset = 0;

    for (size_t i = 0; i < createInfo.nBlocks; ++i) {

      std::string &name = createInfo.pBlockNames[i];

      Block block;
      block.offset = offset;
      block.size = round(createInfo.pBlockSizes[i]);

      m_blocks[name] = block;

      offset += block.size;
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, createInfo.bindingIndex, m_ID);
  }

  void updateSubdata(const std::string &name, const glm::vec3 &data) {
    Block &block = m_blocks[name];
    glNamedBufferSubData(m_ID, block.offset, block.size, glm::value_ptr(data));
  }

  void updateSubdata(const std::string &name, const glm::mat4 &data) {
    Block &block = m_blocks[name];
    glNamedBufferSubData(m_ID, block.offset, block.size, glm::value_ptr(data));
  }

private:
  std::map<std::string, Block> m_blocks;
};

} // namespace gpu

#endif // UNIFORM_BUFFER_H