
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

int currentFilter = 0;

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f), aspect, 0.1f,
                 100.0f};

Shader shader;
Shader screenShader;

GLuint cubeVAO;
GLuint planeVAO;

GLuint marbleTex;
GLuint metalTex;

void process_input(GLFWwindow *window);

void GLAPIENTRY message_callback(GLenum source, GLenum type, GLuint id,
                                 GLenum severity, GLsizei length,
                                 const GLchar *message, const void *userParam);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void cursorPosCallback(GLFWwindow *window, double xPos, double yPos);

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

void drawScene();

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
  float quadVertices[] = {
      // pos(x, y)  // texcoords
      -1.0f,  1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 1.0f, 0.0f,

       1.0f, -1.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f, 1.0f
  };

  // clang-format on

  GLuint cubeVBO;
  glCreateBuffers(1, &cubeVBO);
  glNamedBufferData(cubeVBO, sizeof(cubeVertices), cubeVertices,
                    GL_STATIC_DRAW);

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

  GLuint planeVBO;
  glCreateBuffers(1, &planeVBO);
  glNamedBufferData(planeVBO, sizeof(planeVertices), planeVertices,
                    GL_STATIC_DRAW);

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

  GLuint quadVBO;
  glCreateBuffers(1, &quadVBO);
  glNamedBufferData(quadVBO, sizeof(quadVertices), quadVertices,
                    GL_STATIC_DRAW);

  GLuint quadVAO;
  glCreateVertexArrays(1, &quadVAO);

  // positions
  glEnableVertexArrayAttrib(quadVAO, 0);
  glVertexArrayAttribFormat(quadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(quadVAO, 0, quadVBO, 0, 4 * sizeof(float));
  glVertexArrayAttribBinding(quadVAO, 0, 0);

  // texcoords
  glEnableVertexArrayAttrib(quadVAO, 1);
  glVertexArrayAttribFormat(quadVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(quadVAO, 1, quadVBO, 2 * sizeof(float),
                            4 * sizeof(float));
  glVertexArrayAttribBinding(quadVAO, 1, 1);

  // framebuffer

  GLuint framebuffer;
  glCreateFramebuffers(1, &framebuffer);

  GLuint texColorBuffer;
  glCreateTextures(GL_TEXTURE_2D, 1, &texColorBuffer);
  glTextureStorage2D(texColorBuffer, 1, GL_RGB8, WIDTH, HEIGHT);

  glTextureParameteri(texColorBuffer, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(texColorBuffer, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texColorBuffer,
                            0);

  GLuint rbo;
  glCreateRenderbuffers(1, &rbo);
  glNamedRenderbufferStorage(rbo, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);

  glNamedFramebufferRenderbuffer(framebuffer, GL_DEPTH_STENCIL_ATTACHMENT,
                                 GL_RENDERBUFFER, rbo);

  if (glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) !=
      GL_FRAMEBUFFER_COMPLETE) {

    std::stringstream ss;
    ss << "Framebuffer is not complete!";

    std::string message = ss.str();

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                         framebuffer, GL_DEBUG_SEVERITY_MEDIUM,
                         message.length(), message.c_str());
  }

  stbi_set_flip_vertically_on_load(true);

  glCreateTextures(GL_TEXTURE_2D, 1, &marbleTex);

  {
    int texWidth;
    int texHeight;
    int texNrChannels;

    std::string texPath = getTexturePath("awesomeface.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

    glTextureParameteri(marbleTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(marbleTex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(marbleTex, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(marbleTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(marbleTex, 1, GL_RGBA8, texWidth, texHeight);
    glTextureSubImage2D(marbleTex, 0, 0, 0, texWidth, texHeight, GL_RGBA,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(marbleTex);

    stbi_image_free(data);
  }

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

  // vsync off
  glfwSwapInterval(0);

  shader = Shader("framebuffers.vs", "framebuffers.fs");
  screenShader =
      Shader("framebuffers-screen-post.vs", "framebuffers-screen-post.fs");

  shader.use();
  shader.setInt("texture0", 0);

  screenShader.use();
  screenShader.setInt("screenTexture", 0);

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

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // rendering

    // first pass

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    drawScene();

    // second pass

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    screenShader.use();
    screenShader.setFloat("mode", static_cast<float>(currentFilter));

    glBindTextureUnit(0, texColorBuffer);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &quadVAO);

  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &planeVBO);
  glDeleteBuffers(1, &quadVBO);

  glDeleteFramebuffers(1, &framebuffer);

  glDeleteTextures(1, &marbleTex);
  glDeleteTextures(1, &metalTex);

  glDeleteTextures(1, &texColorBuffer);

  glDeleteRenderbuffers(1, &rbo);

  glDeleteProgram(shader.getID());

  glfwTerminate();
  return 0;
}

void drawScene() {

  const glm::mat4 &view = camera.getViewMatrix();
  const glm::mat4 &projection = camera.getProjectionMatrix();

  shader.use();
  shader.setMat4("view", view);
  shader.setMat4("projection", projection);

  // cubes
  {
    static glm::vec3 cubePositions[] = {glm::vec3{-1.0f, 0.0f, -1.0f},
                                        glm::vec3{2.0f, 0.0f, 0.0f}};

    glBindVertexArray(cubeVAO);
    glBindTextureUnit(0, marbleTex);

    for (int i = 0; i < 2; ++i) {

      glm::mat4 model = glm::mat4{1.0f};
      model = glm::translate(model, cubePositions[i]);
      shader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }

  // floor
  {
    glBindVertexArray(planeVAO);
    glBindTextureUnit(0, metalTex);

    glm::mat4 model = glm::mat4{1.0f};
    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void process_input(GLFWwindow *window) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
    currentFilter = 0;
  } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    currentFilter = 1;
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    currentFilter = 2;
  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
    currentFilter = 3;
  } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
    currentFilter = 4;
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
