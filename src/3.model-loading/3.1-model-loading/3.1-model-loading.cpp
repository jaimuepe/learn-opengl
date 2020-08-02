
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

    // positions        
    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f, 

    -0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f, 

    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f, 

    -0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f, 
     0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f, -0.5f,  

    -0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f, 
  };

  glm::vec3 lightPositions[] = {
    glm::vec3{0.7f, 0.2f, 2.0f},
    glm::vec3{2.3f, -3.3f, -4.0f},
    glm::vec3{-4.0f, 2.0f, -12.0f},
    glm::vec3{0.0f, 0.0f, -3.0f}
  };;

  // clang-format on

  PointLight pointLights[4];

  for (int i = 0; i < 4; ++i) {

    pointLights[i].position = lightPositions[i];

    pointLights[i].ambient = glm::vec3{0.01f, 0.01f, 0.01f};
    pointLights[i].diffuse = glm::vec3{0.2f, 0.1f, 0.3f};
    pointLights[i].specular = glm::vec3{1.0f, 1.0f, 1.0f};

    pointLights[i].constantAtt = 1.0f;
    pointLights[i].linearAtt = 0.09f;
    pointLights[i].quadraticAtt = 0.032f;
  }

  GLuint lightCubeVBO;
  glCreateBuffers(1, &lightCubeVBO);
  glNamedBufferData(lightCubeVBO, 36 * 3 * sizeof(float), cubeVertices,
                    GL_STATIC_DRAW);

  GLuint lightCubeVAO;
  glCreateVertexArrays(1, &lightCubeVAO);

  // positions
  glEnableVertexArrayAttrib(lightCubeVAO, 0);
  glVertexArrayAttribFormat(lightCubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(lightCubeVAO, 0, lightCubeVBO, 0,
                            3 * sizeof(float));
  glVertexArrayAttribBinding(lightCubeVAO, 0, 0);

  glBindVertexArray(0);

  Shader lightingShader("multiple-lights.vs", "multiple-lights.fs");
  Shader lightCubeShader("light-cube.vs", "light-cube.fs");

  stbi_set_flip_vertically_on_load(true);

  std::stringstream modelPath;
  modelPath << getModelPath("backpack") << separator << "backpack.obj";

  Model backpack{modelPath.str()};

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

    for (int i = 0; i < 4; ++i) {

      pointLights[i].diffuse.r =
          0.5f + 0.5f * glm::cos(i + 0.2f * timeSinceStart);

      pointLights[i].diffuse.g =
          0.5f + 0.5f * glm::sin(i + 0.45f * timeSinceStart);

      pointLights[i].diffuse.b =
          0.5f + 0.5f * glm::cos(i + 0.3f * timeSinceStart);
    }

    const glm::mat4 &view = camera.getViewMatrix();
    const glm::mat4 &projection = camera.getProjectionMatrix();

    // cube lights

    lightCubeShader.use();

    lightCubeShader.setMat4("view", view);
    lightCubeShader.setMat4("projection", projection);

    glBindVertexArray(lightCubeVAO);

    for (int i = 0; i < 4; ++i) {

      glm::mat4 model = glm::mat4{1.0f};
      model = glm::translate(model, pointLights[i].position);
      model = glm::scale(model, glm::vec3{0.1f});

      lightCubeShader.setMat4("model", model);
      lightCubeShader.setVec3("lightColor", pointLights[i].diffuse);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    lightingShader.use();

    lightingShader.setFloat("material.shininess", 32.0f);

    glm::vec3 cameraPos = camera.getPosition();
    glm::vec3 cameraFwd = camera.getForward();

    // directional light

    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

    lightingShader.setVec3("dirLight.ambient", 0.02f, 0.02f, 0.02f);
    lightingShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    lightingShader.setVec3("viewPos", cameraPos);

    glm::mat4 model = glm::mat4{1.0f};

    lightingShader.setMat4("model", model);
    lightingShader.setMat4("view", view);
    lightingShader.setMat4("projection", projection);

    lightingShader.setMat4("invModel", glm::inverse(model));

    // point lights

    for (int i = 0; i < 4; ++i) {

      lightingShader.setVec3("pointLights[" + std::to_string(i) + "].position",
                             pointLights[i].position);

      lightingShader.setVec3("pointLights[" + std::to_string(i) + "].ambient",
                             pointLights[i].ambient);

      lightingShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse",
                             pointLights[i].diffuse);

      lightingShader.setVec3("pointLights[" + std::to_string(i) + "].specular",
                             pointLights[i].specular);

      lightingShader.setFloat("pointLights[" + std::to_string(i) +
                                  "].constantAtt",
                              pointLights[i].constantAtt);

      lightingShader.setFloat("pointLights[" + std::to_string(i) +
                                  "].linearAtt",
                              pointLights[i].linearAtt);

      lightingShader.setFloat("pointLights[" + std::to_string(i) +
                                  "].quadraticAtt",
                              pointLights[i].quadraticAtt);
    }

    // spotlight

    /*
        lightingShader.setVec3("spotLight.position", cameraPos);
        lightingShader.setVec3("spotLight.direction", cameraFwd);

        lightingShader.setFloat("spotLight.innerCutOff",
                                glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff",
                                glm::cos(glm::radians(15.0f)));

        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

        lightingShader.setFloat("spotLight.constantAtt", 1.0f);
        lightingShader.setFloat("spotLight.linearAtt", 0.09f);
        lightingShader.setFloat("spotLight.quadraticAtt", 0.032f);
    */

    backpack.draw(lightingShader);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(lightingShader.getID());

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
