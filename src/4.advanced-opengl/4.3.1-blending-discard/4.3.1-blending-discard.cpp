
#include "flycamera.h"
#include "model.h"
#include "pointlight.h"
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

constexpr int WIDTH = 1360;
constexpr int HEIGHT = 768;

float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f), aspect, 0.1f,
                 100.0f};

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

  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // clang-format off


  float cubeVertices[] = {
      // positions         // texture Coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 
       0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
       0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 
       0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 
       0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
       0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 
       0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
      -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 
      -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

      -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 
      -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 
      -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

       0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
       0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
       0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
       0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
       0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
       0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
       0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
       0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 
       0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
      -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 
       0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
       0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
       0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
      -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 
      -0.5f,  0.5f, -0.5f, 0.0f, 1.0f
  };

  float planeVertices[] = {
      // positions         // texture Coords
       5.0f, -0.5f,  5.0f, 2.0f, 0.0f,  
      -5.0f, -0.5f,  5.0f, 0.0f, 0.0f,  
      -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,

       5.0f, -0.5f,  5.0f, 2.0f, 0.0f, 
      -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, 
       5.0f, -0.5f, -5.0f, 2.0f, 2.0f
  };

  float grassVertices[] = {
      // positions         // texture Coords
      0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
      1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

      0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.5f,  0.0f,  1.0f,  1.0f
  };

  glm::vec3 cubePositions[] = {
    glm::vec3(-1.0f, 0.0f, -1.0f),
    glm::vec3(2.0f, 0.0f, 0.0f)
  };

  std::vector<glm::vec3> vegetation;
  vegetation.push_back(glm::vec3{-1.5f, 0.0f, -0.48f});
  vegetation.push_back(glm::vec3{ 1.5f, 0.0f,  0.51f});
  vegetation.push_back(glm::vec3{ 0.0f, 0.0f,  0.7f});
  vegetation.push_back(glm::vec3{-0.3f, 0.0f, -2.3f});
  vegetation.push_back(glm::vec3{ 0.5f, 0.0f, -0.6f});

  // clang-format on

  // cubes

  GLuint cubeVBO;
  glCreateBuffers(1, &cubeVBO);
  glNamedBufferData(cubeVBO, sizeof(cubeVertices), cubeVertices,
                    GL_STATIC_DRAW);

  GLuint cubeVAO;
  glCreateVertexArrays(1, &cubeVAO);

  // positions
  glEnableVertexArrayAttrib(cubeVAO, 0);
  glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(cubeVAO, 0, cubeVBO, 0, 5 * sizeof(float));
  glVertexArrayAttribBinding(cubeVAO, 0, 0);

  // texcoords
  glEnableVertexArrayAttrib(cubeVAO, 1);
  glVertexArrayAttribFormat(cubeVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(cubeVAO, 1, cubeVBO, 3 * sizeof(float),
                            5 * sizeof(float));
  glVertexArrayAttribBinding(cubeVAO, 1, 1);

  // grass

  GLuint grassVBO;
  glCreateBuffers(1, &grassVBO);
  glNamedBufferData(grassVBO, sizeof(grassVertices), grassVertices,
                    GL_STATIC_DRAW);

  GLuint grassVAO;
  glCreateVertexArrays(1, &grassVAO);

  // positions
  glEnableVertexArrayAttrib(grassVAO, 0);
  glVertexArrayAttribFormat(grassVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(grassVAO, 0, grassVBO, 0, 5 * sizeof(float));
  glVertexArrayAttribBinding(grassVAO, 0, 0);

  // texcoords
  glEnableVertexArrayAttrib(grassVAO, 1);
  glVertexArrayAttribFormat(grassVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(grassVAO, 1, grassVBO, 3 * sizeof(float),
                            5 * sizeof(float));
  glVertexArrayAttribBinding(grassVAO, 1, 1);

  // plane

  GLuint planeVBO;
  glCreateBuffers(1, &planeVBO);
  glNamedBufferData(planeVBO, sizeof(planeVertices), planeVertices,
                    GL_STATIC_DRAW);

  GLuint planeVAO;
  glCreateVertexArrays(1, &planeVAO);

  // positions
  glEnableVertexArrayAttrib(planeVAO, 0);
  glVertexArrayAttribFormat(planeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(planeVAO, 0, planeVBO, 0, 5 * sizeof(float));
  glVertexArrayAttribBinding(planeVAO, 0, 0);

  // texcoords
  glEnableVertexArrayAttrib(planeVAO, 1);
  glVertexArrayAttribFormat(planeVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(planeVAO, 1, planeVBO, 3 * sizeof(float),
                            5 * sizeof(float));
  glVertexArrayAttribBinding(planeVAO, 1, 1);

  glBindVertexArray(0);

  stbi_set_flip_vertically_on_load(true);

  GLuint grassTex;
  glCreateTextures(GL_TEXTURE_2D, 1, &grassTex);
  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("grass.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    // clamp_to_edge to avoid artifacts when transparent
    glTextureParameteri(grassTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(grassTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureParameteri(grassTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(grassTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(grassTex, 1, GL_RGBA8, texWidth, texHeight);
    glTextureSubImage2D(grassTex, 0, 0, 0, texWidth, texHeight, GL_RGBA,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(grassTex);

    stbi_image_free(data);
  }

  GLuint marbleTex;
  glCreateTextures(GL_TEXTURE_2D, 1, &marbleTex);
  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("marble.jpg");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(marbleTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(marbleTex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(marbleTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(marbleTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(marbleTex, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(marbleTex, 0, 0, 0, texWidth, texHeight, GL_RGB,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(marbleTex);

    stbi_image_free(data);
  }

  GLuint metalTex;
  glCreateTextures(GL_TEXTURE_2D, 1, &metalTex);
  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("metal.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(metalTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(metalTex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(metalTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(metalTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(metalTex, 1, GL_RGB8, texWidth, texHeight);
    glTextureSubImage2D(metalTex, 0, 0, 0, texWidth, texHeight, GL_RGB,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(metalTex);

    stbi_image_free(data);
  }

  glEnable(GL_DEPTH_TEST);

  // vsync off
  glfwSwapInterval(0);

  Shader shader("blending.vs", "blending.fs");

  shader.use();
  shader.setInt("texture0", 0);

  glUseProgram(0);

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

    const glm::mat4 &view = camera.getViewMatrix();
    const glm::mat4 &projection = camera.getProjectionMatrix();

    // floor
    {
      shader.use();
      shader.setMat4("view", view);
      shader.setMat4("projection", projection);

      glBindVertexArray(planeVAO);
      glBindTextureUnit(0, metalTex);

      glm::mat4 model = glm::mat4{1.0f};
      shader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 6);

      glUseProgram(0);
      glBindVertexArray(0);
    }

    // cubes
    {

      glBindTextureUnit(0, marbleTex);

      shader.use();
      shader.setMat4("view", view);
      shader.setMat4("projection", projection);

      glBindVertexArray(cubeVAO);

      for (int i = 0; i < 2; ++i) {

        glm::mat4 model = glm::mat4{1.0f};
        model = glm::translate(model, cubePositions[i]);

        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      glUseProgram(0);
      glBindVertexArray(0);
    }

    // grass
    {

      glBindVertexArray(grassVAO);
      glBindTextureUnit(0, grassTex);

      shader.use();
      shader.setMat4("view", view);
      shader.setMat4("projection", projection);

      for (unsigned int i = 0; i < vegetation.size(); ++i) {

        glm::mat4 model = glm::mat4{1.0f};
        model = glm::translate(model, vegetation[i]);

        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
      }

      glUseProgram(0);
      glBindVertexArray(0);
    }

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &grassVAO);
  glDeleteVertexArrays(1, &planeVAO);

  glDeleteBuffers(1, &grassVBO);
  glDeleteBuffers(1, &planeVBO);

  glDeleteTextures(1, &grassTex);
  glDeleteTextures(1, &metalTex);

  glDeleteProgram(shader.getID());

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
