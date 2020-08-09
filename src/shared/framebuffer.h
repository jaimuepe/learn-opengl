
#ifndef GPU_FRAMEBUFFER_H
#define GPU_FRAMEBUFFER_H

#include "gpuconstants.h"
#include "gpuobject.h"
#include "renderbuffer.h"
#include "texture.h"

#include <glad/glad.h>

#include <sstream>
#include <string>

namespace gpu {

namespace {
constexpr int MAX_COLOR_ATTACHMENTS = 32;
}

namespace framebuffer {

class Framebuffer : public GpuObject {

public:
  Framebuffer() {}

  inline void setColorAttachment(const texture::Texture &tex,
                                 int colorAttachmentIdx) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    assert(colorAttachmentIdx < MAX_COLOR_ATTACHMENTS);

    glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + colorAttachmentIdx,
                              tex.getID(), 0);
    m_colorAttachmentIDs[colorAttachmentIdx] = tex.getID();
  };

  inline void setColorAttachment(const Renderbuffer &rbo,
                                 int colorAttachmentIdx) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    assert(colorAttachmentIdx < MAX_COLOR_ATTACHMENTS);
    glNamedFramebufferRenderbuffer(m_ID,
                                   GL_COLOR_ATTACHMENT0 + colorAttachmentIdx,
                                   GL_RENDERBUFFER, rbo.getID());
    m_colorAttachmentIDs[colorAttachmentIdx] = rbo.getID();
  };

  inline void setDepthAttachment(const texture::Texture &tex) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, tex.getID(), 0);
    m_depthAttachmentID = tex.getID();
  };

  inline void setDepthAttachment(const Renderbuffer &rbo) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo.getID());
    m_depthAttachmentID = rbo.getID();
  };

  inline void setStencilAttachment(const texture::Texture &tex) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    glNamedFramebufferTexture(m_ID, GL_STENCIL_ATTACHMENT, tex.getID(), 0);
    m_stencilAttachmentID = tex.getID();
  };

  inline void setStencilAttachment(const Renderbuffer &rbo) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    glNamedFramebufferRenderbuffer(m_ID, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo.getID());
    m_stencilAttachmentID = rbo.getID();
  };

  inline void setDepthStencilAttachment(const texture::Texture &tex) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    glNamedFramebufferTexture(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, tex.getID(),
                              0);
    m_depthAttachmentID = tex.getID();
    m_stencilAttachmentID = tex.getID();
  };

  inline void setDepthStencilAttachment(const Renderbuffer &rbo) {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER, rbo.getID());
    m_depthAttachmentID = rbo.getID();
    m_stencilAttachmentID = rbo.getID();
  };

  inline unsigned int getColorAttachmentID(int colorAttachmentIdx) const {
    assert(colorAttachmentIdx < MAX_COLOR_ATTACHMENTS);
    return m_colorAttachmentIDs[colorAttachmentIdx];
  }

  inline unsigned int getDepthAttachmentID() const {
    return m_depthAttachmentID;
  }
  inline unsigned int getStencilAttachmentID() const {
    return m_stencilAttachmentID;
  }

  bool checkStatus() {

    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)

    GLuint status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {

      std::stringstream ss;
      ss << "Framebuffer is not complete! reason: " << status << " (";

      switch (status) {
      case GL_FRAMEBUFFER_UNDEFINED:
        ss << "FRAMEBUFFER_UNDEFINED";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        ss << "FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        ss << "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        ss << "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        ss << "FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        ss << "FRAMEBUFFER_UNSUPPORTED";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        ss << "FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        ss << "FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
        break;
      default:
        ss << "UNKNOWN";
        break;
      }

      ss << ")";

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           m_ID, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());

      return false;
    }

    return true;
  }

  inline void bind() {
    GPU_OBJECT_CREATE_LAZY(glCreateFramebuffers)
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
  }

  virtual void destroy() override {
    glDeleteFramebuffers(1, &m_ID);
    m_ID = 0;
  }

private:
  unsigned int m_colorAttachmentIDs[MAX_COLOR_ATTACHMENTS];
  unsigned int m_depthAttachmentID;
  unsigned int m_stencilAttachmentID;
};

inline void bindDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

inline void setViewport(size_t x, size_t y, size_t width, size_t height) {
  glViewport(x, y, width, height);
}

inline void setClearColor(float r, float g, float b, float a = 1.0f) {
  glClearColor(r, g, b, a);
}

inline void clear(ClearFlagBits flagBits) {
  glClear(static_cast<unsigned int>(flagBits));
}

} // namespace framebuffer

} // namespace gpu

#endif // GPU_FRAMEBUFFER_H