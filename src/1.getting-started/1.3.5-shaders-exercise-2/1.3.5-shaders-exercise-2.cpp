
#include "shader.h"

#include <glad\glad.h>

#include <GLFW\glfw3.h>

#include <iostream>

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

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);

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

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // clang-format off

  float vertices[] = {
      // positions          // colors
      -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
       0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
  };

  // clang-format on

  GLuint VBO;
  glCreateBuffers(1, &VBO);

  glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint VAO;
  glCreateVertexArrays(1, &VAO);

  // positions

  // enable attrib 0
  glEnableVertexArrayAttrib(VAO, 0);

  // attrib 0 has 3 values of float size, no relative offset
  glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

  // binding with VBO at index 0, no offset, stride = 6 float
  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 6 * sizeof(float));

  // binding VBO at 0 with attrib at 0
  glVertexArrayAttribBinding(VAO, 0, 0);

  // colors

  // enable attrib 1
  glEnableVertexArrayAttrib(VAO, 1);

  // attrib 1 has 3 values of float size, no relative offset
  glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 0);

  // binding with VBO at index 1, offset = 3 float, stride = 6 float
  glVertexArrayVertexBuffer(VAO, 1, VBO, 3 * sizeof(float), 6 * sizeof(float));

  // binding VBO at 1 with attrib at 1
  glVertexArrayAttribBinding(VAO, 1, 1);

  glBindVertexArray(0);

  gpu::Shader shader("shader.vs", "shader.fs");

  while (!glfwWindowShouldClose(window)) {

    // input
    process_input(window);

    // rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float time = (float)glfwGetTime();
    float offset = sin(time) * 0.5f;

    shader.setFloat("xOffset", offset);
    shader.use();

    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

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
    break;
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