
#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera {

public:
  Camera(const glm::vec3 &pos, float fov, float aspect, float zNear, float zFar)
      : m_position(pos), m_forward(glm::vec3{0.0f, 0.0f, -1.0f}),
        m_up(glm::vec3{0.0f, 1.0f, 0.0f}) {

    m_projection = glm::perspective(fov, aspect, zNear, zFar);
  }

  inline void setPosition(const glm::vec3 &position) { m_position = position; }

  inline void lookAt(const glm::vec3 &position) {
    m_forward = position - m_position;
  }

  /**
   * Transforms a direction from local-space to world-space.
   */
  inline glm::vec3 transformDirection(const glm::vec3 &dir) const {
    return getViewMatrix() * glm::vec4{dir.x, dir.y, dir.z, 0.0f};
  }

  inline void translate(const glm::vec3 &translation) {
    m_position += translation;
  }

  inline void translateForward(float distance) {
    m_position += m_forward * distance;
  }

  inline void translateRight(float distance) {
    glm::vec3 right = glm::cross(m_forward, m_up);
    m_position += right * distance;
  }

  inline glm::vec3 getForward() const { return m_forward; }
  inline glm::vec3 getRight() const { return glm::cross(m_forward, m_up); }

  inline glm::mat4 getProjectionMatrix() const { return m_projection; }

  inline glm::mat4 getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_forward, m_up);
  }

  inline glm::mat4 getViewProjectionMatrix() {
    return getProjectionMatrix() * getViewMatrix();
  }

private:
  glm::vec3 m_position;

  glm::vec3 m_forward;
  glm::vec3 m_up;

  glm::mat4 m_projection;
};

#endif // CAMERA_H