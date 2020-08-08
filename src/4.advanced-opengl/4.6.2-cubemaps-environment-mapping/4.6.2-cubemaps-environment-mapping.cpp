
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

gpu::Shader *shader;
gpu::Shader reflectShader;
gpu::Shader refractShader;

float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f), aspect, 0.1f,
                 100.0f};

void process_input(GLFWwindow *window);

GLuint loadCubemap(const std::string &name);

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

  float skyboxVertices[] = {
      // positions          
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
  };

  // clang-format on

  GLuint skyboxVBO;
  glCreateBuffers(1, &skyboxVBO);
  glNamedBufferData(skyboxVBO, sizeof(skyboxVertices), skyboxVertices,
                    GL_STATIC_DRAW);

  GLuint skyboxVAO;
  glCreateVertexArrays(1, &skyboxVAO);

  // positions
  glEnableVertexArrayAttrib(skyboxVAO, 0);
  glVertexArrayAttribFormat(skyboxVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(skyboxVAO, 0, skyboxVBO, 0, 3 * sizeof(float));
  glVertexArrayAttribBinding(skyboxVAO, 0, 0);

  glBindVertexArray(0);

  stbi_set_flip_vertically_on_load(true);

  GLuint metalTex;
  {
    glCreateTextures(GL_TEXTURE_2D, 1, &metalTex);

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

  GLuint skyBox = loadCubemap("coast");

  glEnable(GL_DEPTH_TEST);
  // GL_LESS won't work with the skybox since we are forcing it to be at z = 1.0
  // in ndc, meaning it will never render properly (z-fight)
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);

  // vsync off
  glfwSwapInterval(0);

  gpu::Shader skyboxShader("skybox.vs", "skybox.fs");

  reflectShader = gpu::Shader{"reflection.vs", "reflection.fs"};
  refractShader = gpu::Shader{"reflection.vs", "refraction.fs"};

  std::stringstream modelPath;
  modelPath << getModelPath("backpack") << separator << "backpack.obj";

  Model backpack = Model{modelPath.str()};

  reflectShader.use();
  reflectShader.setInt("skybox", 0);

  refractShader.use();
  refractShader.setInt("skybox", 0);

  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  shader = &reflectShader;

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

    shader->use();

    shader->setMat4("model", glm::mat4{1.0});
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    shader->setVec3("cameraPos", camera.getPosition());

    glBindTextureUnit(0, skyBox);

    backpack.draw(*shader);

    // skybox
    {
      skyboxShader.use();

      // remove the translation part!
      skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
      skyboxShader.setMat4("projection", projection);

      glBindVertexArray(skyboxVAO);
      glBindTextureUnit(0, skyBox);

      glDrawArrays(GL_TRIANGLES, 0, 36);

      glUseProgram(0);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteProgram(refractShader.getID());
  glDeleteProgram(reflectShader.getID());

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow *window) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    shader = &reflectShader;
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    shader = &refractShader;
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

GLuint loadCubemap(const std::string &name) {

  stbi_set_flip_vertically_on_load(false);

  GLuint cubemap;
  {
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap);

    glTextureParameteri(cubemap, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(cubemap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(cubemap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::vector<std::string> faces = {"right.jpg",  "left.jpg",  "top.jpg",
                                      "bottom.jpg", "front.jpg", "back.jpg"};

    int texWidth;
    int texHeight;
    int texNrChannels;

    for (unsigned int i = 0; i < 6; ++i) {

      std::string path = getSkyboxPath(name, faces[i]);

      unsigned char *data =
          stbi_load(path.c_str(), &texWidth, &texHeight, &texNrChannels, 0);

      if (i == 0) {
        glTextureStorage2D(cubemap, 1, GL_RGB8, texWidth, texHeight);
      }

      glTextureSubImage3D(cubemap, 0, 0, 0, i, texWidth, texHeight, 1, GL_RGB,
                          GL_UNSIGNED_BYTE, data);

      stbi_image_free(data);
    }
  }

  stbi_set_flip_vertically_on_load(true);

  return cubemap;
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
