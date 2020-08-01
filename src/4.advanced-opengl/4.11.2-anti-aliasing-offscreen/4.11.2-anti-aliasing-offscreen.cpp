
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

constexpr int WIDTH = 1600;
constexpr int HEIGHT = 600;

float aspect = static_cast<float>(WIDTH * 0.5f) / static_cast<float>(HEIGHT);

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

  // cube

  GLuint cubeVAO;
  GLuint cubeVBO;
  {
    glCreateBuffers(1, &cubeVBO);
    glNamedBufferData(cubeVBO, sizeof(cubeVertices), cubeVertices,
                      GL_STATIC_DRAW);

    glCreateVertexArrays(1, &cubeVAO);

    // positions
    glEnableVertexArrayAttrib(cubeVAO, 0);
    glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(cubeVAO, 0, cubeVBO, 0, 5 * sizeof(float));
    glVertexArrayAttribBinding(cubeVAO, 0, 0);
  }

  // quad

  GLuint quadVAO;
  GLuint quadVBO;
  {
    glCreateVertexArrays(1, &quadVAO);
    glCreateBuffers(1, &quadVBO);

    glNamedBufferData(quadVBO, sizeof(quadVertices), quadVertices,
                      GL_STATIC_DRAW);

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
  }

  // off-screen rendering

  GLuint msFbo;
  GLuint msTexColorBuffer;
  GLuint msRbo;
  {
    glCreateFramebuffers(1, &msFbo);

    glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &msTexColorBuffer);
    glTextureStorage2DMultisample(msTexColorBuffer, 4, GL_RGB8, WIDTH, HEIGHT,
                                  GL_TRUE);

    glNamedFramebufferTexture(msFbo, GL_COLOR_ATTACHMENT0, msTexColorBuffer, 0);

    glCreateRenderbuffers(1, &msRbo);
    glNamedRenderbufferStorageMultisample(msRbo, 4, GL_DEPTH24_STENCIL8, WIDTH,
                                          HEIGHT);

    glNamedFramebufferRenderbuffer(msFbo, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_RENDERBUFFER, msRbo);

    if (glCheckNamedFramebufferStatus(msFbo, GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {

      std::stringstream ss;
      ss << "Framebuffer is not complete!";

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           msFbo, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());
    }
  }

  // intermediate fbo

  GLuint intFbo;
  GLuint intTexColorBuffer;
  {
    glCreateFramebuffers(1, &intFbo);

    glCreateTextures(GL_TEXTURE_2D, 1, &intTexColorBuffer);
    glTextureStorage2D(intTexColorBuffer, 1, GL_RGB8, WIDTH, HEIGHT);
    glNamedFramebufferTexture(intFbo, GL_COLOR_ATTACHMENT0, intTexColorBuffer,
                              0);

    if (glCheckNamedFramebufferStatus(intFbo, GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {

      std::stringstream ss;
      ss << "Framebuffer is not complete!";

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           intFbo, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                           message.c_str());
    }
  }

  // vsync off
  glfwSwapInterval(0);

  Shader shader("flat-color.vs", "flat-color.fs");
  Shader screenShader("screen-quad.vs", "screen-quad.fs");

  shader.use();
  shader.setVec3("color", glm::vec3{0.0f, 1.0f, 0.0f});

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

    const glm::mat4 &view = camera.getViewMatrix();
    const glm::mat4 &projection = camera.getProjectionMatrix();

    // rendering

    // 1) we render to an offscreen buffer so we can use antialiasing (not using
    // the one provided by glfw).
    // 2) we blit this to another intermediate
    // framebuffer so we can then use its texture in the main framebuffer and
    // apply some post processing
    // 3) we render the intermediate texture in a
    // quad and do some post process (black&white) in the quad's shader

    // 1) first pass - offscreen
    {
      glBindFramebuffer(GL_FRAMEBUFFER, msFbo);

      glEnable(GL_DEPTH_TEST);

      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // cube
      {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glm::mat4 model = glm::mat4{1.0f};
        shader.setMat4("model", model);

        glBindVertexArray(cubeVAO);

        // left (no AA)

        glViewport(0, 0, WIDTH / 2, HEIGHT);
        glDisable(GL_MULTISAMPLE);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // right (AA)

        glViewport(WIDTH / 2, 0, WIDTH / 2, HEIGHT);
        glEnable(GL_MULTISAMPLE);

        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glDisable(GL_MULTISAMPLE);

    // 2) blit to intermediate buffer
    {
      glBlitNamedFramebuffer(msFbo, intFbo, 0, 0, WIDTH, HEIGHT, 0, 0, WIDTH,
                             HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // 3) copy to main framebuffer, post-processing
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glDisable(GL_DEPTH_TEST);

      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      glBindVertexArray(quadVAO);

      glBindTextureUnit(0, intTexColorBuffer);
      screenShader.use();

      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);

  glDeleteBuffers(1, &cubeVBO);

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
