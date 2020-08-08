
#ifndef GPU_FRAMEBUFFER_H
#define GPU_FRAMEBUFFER_H

#include "gpuobject.h"
#include "renderbuffer.h"
#include "texture2d.h"

#include <glad/glad.h>

#include <sstream>
#include <string>

namespace gpu {

class Framebuffer : public GpuObject {

public:
  Framebuffer() { glCreateFramebuffers(1, &m_ID); }

  inline void attachColor(const Texture2D &tex, int colorAttachmentIdx) const {
    glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + colorAttachmentIdx,
                              tex.getID(), 0);
  };

  inline void attachColor(const Renderbuffer &rbo,
                          int colorAttachmentIdx) const {
    glNamedFramebufferRenderbuffer(m_ID,
                                   GL_COLOR_ATTACHMENT0 + colorAttachmentIdx,
                                   GL_RENDERBUFFER, rbo.getID());
  };

  inline void attachDepth(const Texture2D &tex) const {
    glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, tex.getID(), 0);
  };

  inline void attachDepth(const Renderbuffer &rbo) const {
    glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo.getID());
  };

  inline void attachStencil(const Texture2D &tex) const {
    glNamedFramebufferTexture(m_ID, GL_STENCIL_ATTACHMENT, tex.getID(), 0);
  };

  inline void attachStencil(const Renderbuffer &rbo) const {
    glNamedFramebufferRenderbuffer(m_ID, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                   rbo.getID());
  };

  inline void attachDepthStencil(const Texture2D &tex) const {
    glNamedFramebufferTexture(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, tex.getID(),
                              0);
  };

  inline void attachDepthStencil(const Renderbuffer &rbo) const {
    glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER, rbo.getID());
  };

  bool checkStatus() const {

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

  inline void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_ID); }

  virtual void destroy() override {
    glDeleteFramebuffers(1, &m_ID);
    m_ID = 0;
  }
};

} // namespace gpu

#endif // GPU_FRAMEBUFFER_H