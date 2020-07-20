
#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera {

public:
  // clang-format off
  
  Camera(
    const glm::vec3 &pos, 
    float fov, 
    float aspect, 
    float zNear, 
    float zFar)
      : m_fov(fov),
        m_aspect(aspect),
        m_zNear(zNear),
        m_zFar(zFar),
        m_pitch(0.0f), 
        m_yaw(-glm::half_pi<float>()), // -90.0 deg
        m_position(pos),
        m_forward(glm::vec3{0.0f, 0.0f, -1.0f}),
        m_up(glm::vec3{0.0f, 1.0f, 0.0f}), 
        m_viewMatrixDirty(true),
        m_projectionMatrixDirty(true) {}

  // clang-format on

  inline void setFov(float fov) {
    m_fov = fov;
    m_projectionMatrixDirty = true;
  }

  inline float getFov() const { return m_fov; }

  inline void lookAt(const glm::vec3 &position) {
    m_forward = position - m_position;
    m_viewMatrixDirty = true;
  }

  /**
   * Transforms a direction from local-space to world-space.
   */
  inline glm::vec3 transformDirection(const glm::vec3 &dir) {
    return getInverseViewMatrix() * glm::vec4{dir.x, dir.y, dir.z, 0.0f};
  }

  inline void rotateTaitBryan(float yaw, float pitch) {

    m_yaw += yaw;
    m_pitch += pitch;

    m_pitch = glm::clamp(m_pitch, -glm::half_pi<float>() + 0.1f,
                         glm::half_pi<float>() - 0.1f);

    glm::vec3 dir;
    dir.x = static_cast<float>(cos(m_yaw)) * static_cast<float>(cos(m_pitch));
    dir.y = static_cast<float>(sin(m_pitch));
    dir.z = static_cast<float>(sin(m_yaw)) * static_cast<float>(cos(m_pitch));

    m_forward = glm::normalize(dir);

    m_viewMatrixDirty = true;
  }

  inline void translate(const glm::vec3 &translation) {
    m_position += translation;
    m_viewMatrixDirty = true;
  }

  inline glm::vec3 getForward() const { return m_forward; }
  inline glm::vec3 getRight() const { return glm::cross(m_forward, m_up); }

  inline glm::mat4 getProjectionMatrix() {

    if (m_projectionMatrixDirty) {
      m_projectionMatrix = glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
      m_projectionMatrixDirty = false;
    }

    return m_projectionMatrix;
  }

  inline glm::mat4 getViewMatrix() {

    if (m_viewMatrixDirty) {
      m_viewMatrix = glm::lookAt(m_position, m_position + m_forward, m_up);
      m_viewMatrixDirty = false;
    }

    return m_viewMatrix;
  }

  inline glm::mat4 getInverseViewMatrix() {

    if (m_viewMatrixDirty || m_inverseViewMatrixDirty) {
      m_inverseViewMatrix = glm::inverse(getViewMatrix());
      m_inverseViewMatrixDirty = false;
    }

    return m_inverseViewMatrix;
  }

  inline glm::mat4 getViewProjectionMatrix() {
    return getProjectionMatrix() * getViewMatrix();
  }

private:
  float m_yaw;
  float m_pitch;

  float m_fov;
  float m_aspect;
  float m_zNear;
  float m_zFar;

  glm::vec3 m_position;

  glm::vec3 m_forward;
  glm::vec3 m_up;

  glm::mat4 m_viewMatrix;
  bool m_viewMatrixDirty;

  glm::mat4 m_inverseViewMatrix;
  bool m_inverseViewMatrixDirty;

  bool m_projectionMatrixDirty;
  glm::mat4 m_projectionMatrix;
};

#endif // CAMERA_H