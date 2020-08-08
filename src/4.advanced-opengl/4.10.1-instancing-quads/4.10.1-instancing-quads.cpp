
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

float lastTime = 0.0f;
float deltaTime = 0.0f;

float fpsCounterTime = 0.0f;
int nrFrames = 0;

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

void process_input(GLFWwindow *window);

void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id,
                                 GLenum severity, GLsizei length,
                                 const GLchar *message, const void *userParam);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main() {

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(message_callback, 0);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // clang-format off

  float quadVertices[] = {
      // positions    // colors
      -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
       0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
      -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,
      -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
       0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
       0.05f,  0.05f, 0.0f, 1.0f, 1.0f
  };

  int index = 0;

  glm::vec2 translations[100];
  for (int i = -10; i < 10; i += 2) {
    for (int j = -10; j < 10; j += 2) {
      glm::vec3 translation;
      translation.x = static_cast<float>(i) * 0.1f + 0.1f;
      translation.y = static_cast<float>(j) * 0.1f + 0.1f;
      translations[index++] = translation;
    }
  }

  // clang-format on

  GLuint quadVBO;
  glCreateBuffers(1, &quadVBO);
  glNamedBufferData(quadVBO, sizeof(quadVertices), quadVertices,
                    GL_STATIC_DRAW);

  GLuint quadVAO;
  glCreateVertexArrays(1, &quadVAO);

  // positions
  glEnableVertexArrayAttrib(quadVAO, 0);
  glVertexArrayAttribFormat(quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(quadVAO, 0, quadVBO, 0, 5 * sizeof(float));
  glVertexArrayAttribBinding(quadVAO, 0, 0);

  // colors
  glEnableVertexArrayAttrib(quadVAO, 1);
  glVertexArrayAttribFormat(quadVAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(quadVAO, 1, quadVBO, 2 * sizeof(float),
                            5 * sizeof(float));
  glVertexArrayAttribBinding(quadVAO, 1, 1);

  GLuint instanceVBO;
  glCreateBuffers(1, &instanceVBO);
  glNamedBufferData(instanceVBO, sizeof(translations), &translations[0],
                    GL_STATIC_DRAW);

  // per-instance offset
  glEnableVertexArrayAttrib(quadVAO, 2);
  glVertexArrayAttribFormat(quadVAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(quadVAO, 2, instanceVBO, 0, 2 * sizeof(float));
  glVertexArrayAttribBinding(quadVAO, 2, 2);
  glVertexArrayBindingDivisor(quadVAO, 2, 1);

  // vsync off
  glfwSwapInterval(0);

gpu::Shader shader("instancing.vs", "instancing.fs");

  while (!glfwWindowShouldClose(window)) {

    float timeSinceStart = static_cast<float>(glfwGetTime());
    deltaTime = timeSinceStart - lastTime;

    fpsCounterTime += deltaTime;

    nrFrames++;

    if (fpsCounterTime > 1.0f) {

      std::stringstream ss;
      ss << "LearnOpenGL"
         << " [" << (1000.0 / static_cast<double>(nrFrames)) << " ms/frame]"
         << " [ " << nrFrames << " FPS]";

      glfwSetWindowTitle(window, ss.str().c_str());

      nrFrames = 0;
      fpsCounterTime = 0.0f;
    }

    lastTime = timeSinceStart;

    // input
    process_input(window);

    // rendering
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // quads
    {
      shader.use();
      glBindVertexArray(quadVAO);
      glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);

  glDeleteProgram(shader.getID());

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id,
                                 GLenum severity, GLsizei length,
                                 const GLchar *message, const void *userParam) {

  std::cout << "---------------------opengl-callback-start------------"
            << std::endl;

  std::cout << "message: " << message << std::endl;
  std::cout << "type: ";
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "ERROR";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "DEPRECATED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "UNDEFINED_BEHAVIOR";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "PORTABILITY";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "PERFORMANCE";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "OTHER";
    break;
  }
  std::cout << std::endl;

  std::cout << "id: " << id << std::endl;
  std::cout << "severity: ";
  switch (severity) {
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "NOTIFICATION";
    return;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "LOW";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "MEDIUM";
    break;
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "HIGH";
    break;
  }
  std::cout << std::endl;

  std::cout << "---------------------opengl-callback-end--------------"
            << std::endl;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
