
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

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

        // position           // texcoords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
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
  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(float));

  // binding VBO at 0 with attrib at 0
  glVertexArrayAttribBinding(VAO, 0, 0);

  // texcoords

  // enable attrib 1
  glEnableVertexArrayAttrib(VAO, 1);

  // attrib 1 has 3 values of float size, no relative offset
  glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 0);

  // binding with VBO at index 1, offset = 3 float, stride = 6 float
  glVertexArrayVertexBuffer(VAO, 1, VBO, 3 * sizeof(float), 5 * sizeof(float));

  // binding VBO at 1 with attrib at 1
  glVertexArrayAttribBinding(VAO, 1, 1);

  glBindVertexArray(0);

  Shader shader("shader.vs", "shader.fs");

  stbi_set_flip_vertically_on_load(true);

  GLuint texture1;
  glCreateTextures(GL_TEXTURE_2D, 1, &texture1);

  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("container.jpg");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(texture1, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture1, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(texture1, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture1, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(texture1, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(texture1, 0, 0, 0, texWidth, texHeight, GL_RGB,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(texture1);

    stbi_image_free(data);
  }

  GLuint texture2;
  glCreateTextures(GL_TEXTURE_2D, 1, &texture2);

  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("awesomeface.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(texture2, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture2, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(texture2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(texture2, 1, GL_RGBA16, texWidth, texHeight);
    glTextureSubImage2D(texture2, 0, 0, 0, texWidth, texHeight, GL_RGBA,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(texture2);

    stbi_image_free(data);
  }

  shader.setInt("texture1", 0);
  shader.setInt("texture2", 1);

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {

    // input
    process_input(window);

    // rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    glm::mat4 view = glm::mat4{1.0f};
    view = glm::translate(view, glm::vec3{0.0f, 0.0f, -3.0f});

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glBindTextureUnit(0, texture1);
    glBindTextureUnit(1, texture2);

    glBindVertexArray(VAO);

    for (int i = 0; i < 10; ++i) {

      glm::mat4 model = glm::mat4{1.0f};
      model = glm::translate(model, cubePositions[i]);

      float angle = 20.0f * i;

      model =
          glm::rotate(model, glm::radians(angle), glm::vec3{1.0f, 0.3f, 0.5f});

      shader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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