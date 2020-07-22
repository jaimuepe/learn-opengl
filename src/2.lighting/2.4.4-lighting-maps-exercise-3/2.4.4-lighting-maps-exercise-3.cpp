
#include "flycamera.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

float cameraSpeed = 3.0f;

float lastTime = 0.0f;
float deltaTime = 0.0f;

float fpsCounterTime = 0.0f;
int nrFrames = 0;

bool firstMouse = true;

float lastMouseX = 400.0f;
float lastMouseY = 300.0f;

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f),
                 800.0f / 600.0f, 0.1f, 100.0f};

void process_input(GLFWwindow *window);

void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id,
                                 GLenum severity, GLsizei length,
                                 const GLchar *message, const void *userParam);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

int main() {

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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

  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(message_callback, 0);

  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // clang-format off

  float vertices[] = {
    // positions          // normals           // texcoords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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
  glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 8 * sizeof(float));
  // binding VBO at 0 with attrib at 0
  glVertexArrayAttribBinding(VAO, 0, 0);

  // normals
  glEnableVertexArrayAttrib(VAO, 1);
  // attrib 0 has 3 values of float size, no relative offset
  glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
  // binding with VBO at index 0, no offset, stride = 6 float
  glVertexArrayVertexBuffer(VAO, 1, VBO, 3 * sizeof(float), 8 * sizeof(float));
  // binding VBO at 0 with attrib at 0
  glVertexArrayAttribBinding(VAO, 1, 1);

  // texcoords
  glEnableVertexArrayAttrib(VAO, 2);
  // attrib 2 has 2 values of float size, no relative offset
  glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
  // binding with VBO at index 2, offset = 6, stride = 8 float
  glVertexArrayVertexBuffer(VAO, 2, VBO, 6 * sizeof(float), 8 * sizeof(float));
  // binding VBO at 0 with attrib at 0
  glVertexArrayAttribBinding(VAO, 2, 2);

  // light VAO

  GLuint lightVAO;
  glCreateVertexArrays(1, &lightVAO);

  glEnableVertexArrayAttrib(lightVAO, 0);
  glVertexArrayAttribFormat(lightVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(lightVAO, 0, VBO, 0, 8 * sizeof(float));
  glVertexArrayAttribBinding(lightVAO, 0, 0);

  glBindVertexArray(0);

  Shader lightingShader("lighting-maps.vs", "lighting-maps.fs");
  Shader lightCubeShader("light-cube.vs", "light-cube.fs");

  stbi_set_flip_vertically_on_load(true);

  GLuint containerDiffuseMap;
  glCreateTextures(GL_TEXTURE_2D, 1, &containerDiffuseMap);

  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("container2.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(containerDiffuseMap, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(containerDiffuseMap, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(containerDiffuseMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(containerDiffuseMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(containerDiffuseMap, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(containerDiffuseMap, 0, 0, 0, texWidth, texHeight,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(containerDiffuseMap);

    stbi_image_free(data);
  }

  GLuint containerSpecularMap;
  glCreateTextures(GL_TEXTURE_2D, 1, &containerSpecularMap);

  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("lighting_maps_specular_color.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(containerSpecularMap, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(containerSpecularMap, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(containerSpecularMap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(containerSpecularMap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(containerSpecularMap, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(containerSpecularMap, 0, 0, 0, texWidth, texHeight,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(containerSpecularMap);

    stbi_image_free(data);
  }

  glm::vec3 lightPos{1.2f, 1.0f, 2.0f};

  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

  glEnable(GL_DEPTH_TEST);

  // vsync off
  glfwSwapInterval(0);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4{1.0f};
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3{0.2f});

    glm::vec3 lightDiffuse = glm::vec3{0.5f, 0.5f, 0.5f};

    lightCubeShader.use();

    lightCubeShader.setMat4("model", model);
    lightCubeShader.setMat4("view", camera.getViewMatrix());
    lightCubeShader.setMat4("projection", camera.getProjectionMatrix());

    lightCubeShader.setVec3("lightColor", lightDiffuse);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4{1.0f};

    glBindTextureUnit(0, containerDiffuseMap);
    glBindTextureUnit(1, containerSpecularMap);

    lightingShader.use();

    lightingShader.setFloat("material.shininess", 32.0f);

    // light properties
    lightingShader.setVec3("light.position", lightPos);
    lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("light.diffuse", lightDiffuse);
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    lightingShader.setVec3("viewPos", camera.getPosition());

    lightingShader.setMat4("model", model);
    lightingShader.setMat4("view", camera.getViewMatrix());
    lightingShader.setMat4("projection", camera.getProjectionMatrix());

    lightingShader.setMat4("invModel", glm::inverse(model));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glDeleteTextures(1, &containerDiffuseMap);
  glDeleteTextures(1, &containerSpecularMap);

  glDeleteProgram(lightingShader.getID());
  glDeleteProgram(lightCubeShader.getID());

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  int front = 0;
  int right = 0;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    // in cam-space, forward-z is negative!
    front = -1;
  } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    front = 1;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    right = -1;
  } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    right = 1;
  }

  if (front != 0 || right != 0) {

    float speed = cameraSpeed * deltaTime;

    glm::vec3 dirCamSpace = glm::vec3{right, 0.0f, front};
    dirCamSpace = glm::normalize(dirCamSpace);

    glm::vec3 dirWorldSpace = camera.transformDirection(dirCamSpace);
    camera.translate(dirWorldSpace * speed);
  }
}

void cursorPosCallback(GLFWwindow *window, double xPos, double yPos) {

  float mouseX = static_cast<float>(xPos);
  float mouseY = static_cast<float>(yPos);

  if (firstMouse) {

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    firstMouse = false;
  }

  float xOffset = mouseX - lastMouseX;
  float yOffset = lastMouseY - mouseY;

  lastMouseX = mouseX;
  lastMouseY = mouseY;

  const float sensitivity = 0.005f;

  xOffset *= sensitivity;
  yOffset *= sensitivity;

  camera.rotateTaitBryan(xOffset, yOffset);
}

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {

  float fov = glm::degrees(camera.getFov()) - static_cast<float>(yOffset);
  fov = glm::clamp(fov, 1.0f, 45.0f);

  camera.setFov(glm::radians(fov));
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
