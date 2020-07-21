
#ifndef FLY_CAMERA_H
#define FLY_CAMERA_H

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class FlyCamera {

public:
  // clang-format off
  
  FlyCamera(
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
        m_viewMatrixDirty(true),
        m_inverseViewMatrixDirty(true),
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
   * Transforms a direction from view-space to world-space.
   */
  inline glm::vec3 transformDirection(const glm::vec3 &dir) {
    return getInverseViewMatrix() * glm::vec4{dir.x, dir.y, dir.z, 0.0f};
  }

  /**
   * Rotates with a specific yaw & pitch (no roll atm).
   */
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

  inline void setPosition(const glm::vec3 &position) {
    m_position = position;
    m_viewMatrixDirty = true;
  }

  inline const glm::vec3 &getPosition() const { return m_position; }

  inline void translate(const glm::vec3 &translation) {
    m_position += translation;
    m_viewMatrixDirty = true;
  }

  inline const glm::mat4 &getProjectionMatrix() {

    if (m_projectionMatrixDirty) {
      m_projectionMatrix = glm::perspective(m_fov, m_aspect, m_zNear, m_zFar);
      m_projectionMatrixDirty = false;
    }

    return m_projectionMatrix;
  }

  inline const glm::mat4 &getViewMatrix() {

    if (m_viewMatrixDirty) {

      /*

      // lookAt matrix (manually)
      // ---------

      //
      //          | Rx  Ry  Rz  0|     | 1  0  0  -Px|
      // lookAt = | Ux  Uy  Uz  0|  *  | 0  1  0  -Py|
      //          | Dx  Dy  Dz  0|     | 0  0  1  -Pz|
      //          |  0   0   0  1|     | 0  0  0    1|
      // Where:
      // R = right (view space)
      // U = up (view space)
      // D = view direction (negated)
      // P = camera position
      //
      // !!! glm/opengl matrices are in column-major order !!!

      glm::vec3 dir = -m_forward;

      glm::vec3 right =
          glm::normalize(glm::cross(glm::vec3{0.0f, 1.0f, 0.0f}, dir));

      glm::vec3 up = glm::cross(dir, right);

      m_viewMatrix = glm::mat4{
        right.x,        up.x,           dir.x,          0.0f,
        right.y,        up.y,           dir.y,          0.0f,
        right.z,        up.z,           dir.z,          0.0f,
        0.0f,           0.0f,           0.0f,           1.0f};

      m_viewMatrix *= glm::mat4{
        1.0f,           0.0f,           0.0f,           0.0f,
        0.0f,           1.0f,           0.0f,           0.0f,
        0.0f,           0.0f,           1.0f,           0.0f,
        -m_position.x,  -m_position.y,  -m_position.z,  1.0f};

      */

      m_viewMatrix = glm::lookAt(m_position, m_position + m_forward,
                                 glm::vec3{0.0f, 1.0f, 0.0f});

      m_viewMatrixDirty = false;
    }

    return m_viewMatrix;
  }

  inline const glm::mat4 &getInverseViewMatrix() {

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

  glm::mat4 m_viewMatrix;
  bool m_viewMatrixDirty;

  glm::mat4 m_inverseViewMatrix;
  bool m_inverseViewMatrixDirty;

  bool m_projectionMatrixDirty;
  glm::mat4 m_projectionMatrix;
};

#endif // FLY_CAMERA_H