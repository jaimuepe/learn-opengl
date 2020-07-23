
#include "flycamera.h"
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

  // positions all containers
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

  PointLight pointLights[4];

  glm::vec3 pointLightPositions[] = {
    glm::vec3{0.7f, 0.2f, 2.0f}, 
    glm::vec3{2.3f, -3.3f, -4.0f},
    glm::vec3{-4.0f, 2.0f, -12.0f}, 
    glm::vec3{0.0f, 0.0f, -3.0f}
  };

  glm::vec3 pointLightColors[] = {
      glm::vec3(1.0f, 0.6f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 1.0, 0.0),
      glm::vec3(0.2f, 0.2f, 1.0f)
  };

  // clang-format on

  for (int i = 0; i < 4; ++i) {

    pointLights[i].position = pointLightPositions[i];
    pointLights[i].ambient = 0.1f * pointLightColors[i];
    pointLights[i].diffuse = pointLightColors[i];
    pointLights[i].specular = pointLightColors[i];

    pointLights[i].constantAtt = 1.0f;
    pointLights[i].linearAtt = 0.09f;
    pointLights[i].quadraticAtt = 0.032f;
  }

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

  GLuint lightVAO;
  glCreateVertexArrays(1, &lightVAO);

  // enable attrib 0
  glEnableVertexArrayAttrib(lightVAO, 0);
  // attrib 0 has 3 values of float size, no relative offset
  glVertexArrayAttribFormat(lightVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  // binding with VBO at index 0, no offset, stride = 6 float
  glVertexArrayVertexBuffer(lightVAO, 0, VBO, 0, 8 * sizeof(float));
  // binding VBO at 0 with attrib at 0
  glVertexArrayAttribBinding(lightVAO, 0, 0);

  glBindVertexArray(0);

  Shader lightingShader("multiple-lights.vs", "multiple-lights.fs");
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

    std::string texPath = getTexturePath("container2_specular.png");

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
    glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 &view = camera.getViewMatrix();
    const glm::mat4 &projection = camera.getProjectionMatrix();

    // cube lights

    lightCubeShader.use();

    lightCubeShader.setMat4("view", view);
    lightCubeShader.setMat4("projection", projection);

    glBindVertexArray(lightVAO);

    for (int i = 0; i < 4; ++i) {

      glm::mat4 model = glm::mat4{1.0f};
      model = glm::translate(model, pointLights[i].position);

      lightCubeShader.setMat4("model", model);
      lightCubeShader.setVec3("lightColor", pointLights[i].diffuse);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindTextureUnit(0, containerDiffuseMap);
    glBindTextureUnit(1, containerSpecularMap);

    lightingShader.use();

    lightingShader.setFloat("material.shininess", 32.0f);

    glm::vec3 cameraPos = camera.getPosition();
    glm::vec3 cameraFwd = camera.getForward();

    // directional light

    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

    lightingShader.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
    lightingShader.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    lightingShader.setVec3("viewPos", cameraPos);

    lightingShader.setMat4("view", view);
    lightingShader.setMat4("projection", projection);

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

    glBindVertexArray(VAO);

    for (int i = 0; i < 10; ++i) {

      glm::mat4 model = glm::mat4{1.0f};
      model = glm::translate(model, cubePositions[i]);

      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3{1.0f, 0.3f, 0.5f});

      lightingShader.setMat4("model", model);
      lightingShader.setMat4("invModel", glm::inverse(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &lightVAO);

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glDeleteTextures(1, &containerDiffuseMap);
  glDeleteTextures(1, &containerSpecularMap);

  glDeleteProgram(lightCubeShader.getID());
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
