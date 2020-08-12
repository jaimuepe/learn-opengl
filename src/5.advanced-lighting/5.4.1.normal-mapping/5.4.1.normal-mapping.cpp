
#include "basicmeshes.h"
#include "cubemap.h"
#include "flycamera.h"
#include "framebuffer.h"
#include "mesh.h"
#include "model.h"
#include "pointlight.h"
#include "renderbuffer.h"
#include "shader.h"
#include "texture2d.h"
#include "uniformbuffer.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

float cameraSpeed = 3.0f;

float currentTime = 0.0f;

float lastTime = 0.0f;
float deltaTime = 0.0f;

float fpsCounterTime = 0.0f;
int nrFrames = 0;

bool firstMouse = true;

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

constexpr int SHADOW_WIDTH = 1024;
constexpr int SHADOW_HEIGHT = 1024;

constexpr int MAX_POINT_LIGHTS = 6;

float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

float shadowAspect =
    static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT);

float lastMouseX = static_cast<float>(WIDTH) * 0.5f;
float lastMouseY = static_cast<float>(HEIGHT) * 0.5f;

gpu::texture::Texture2D wallDiffuseTexture;
gpu::texture::Texture2D blackUnitTexture;
gpu::texture::Texture2D whiteUnitTexture;
gpu::texture::Texture2D wallNormalTexture;

Mesh cubeMesh;

Model suzzane;

size_t nActiveLights = 1;

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f), aspect, 0.1f,
                 100.0f};

std::vector<PointLight> pointLights;

gpu::Shader lightCubeShader;

void drawLightCubes();
void drawScene(const gpu::Shader &shader);

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

  // shadow mapping

  // omni

  gpu::framebuffer::Framebuffer depthMapOmniFramebuffers[MAX_POINT_LIGHTS];
  for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i) {

    auto &framebuffer = depthMapOmniFramebuffers[i];
    gpu::texture::Cubemap depthCubemap{SHADOW_WIDTH, SHADOW_HEIGHT,
                                       GL_DEPTH_COMPONENT16};

    depthCubemap.setMinMagFilter(gpu::texture::Filter::NEAREST);
    depthCubemap.setWrapRST(gpu::texture::Wrap::CLAMP_TO_EDGE);

    framebuffer.setDepthAttachment(depthCubemap);
    framebuffer.checkStatus();
  }

  // uniform buffers

  gpu::UniformBufferCreateInfo uboCreateInfo;

  uboCreateInfo.bindingIndex = 0;
  uboCreateInfo.nBlocks = 3;

  std::string blockNames[] = {"cameraPosition", "cameraView",
                              "cameraProjection"};

  size_t blockSizes[] = {sizeof(glm::vec3), sizeof(glm::mat4),
                         sizeof(glm::mat4)};

  uboCreateInfo.pBlockNames = blockNames;
  uboCreateInfo.pBlockSizes = blockSizes;

  gpu::UniformBuffer camUniformBuffer{uboCreateInfo};

  cubeMesh = createCube();

  wallDiffuseTexture = gpu::texture::Texture2D{"brickwall.jpg"};

  wallNormalTexture = gpu::texture::Texture2D{"brickwall_normal.jpg"};

  blackUnitTexture =
      gpu::texture::createUnitTexture2D(glm::vec3{0.0f, 0.0f, 0.0f});

  whiteUnitTexture =
      gpu::texture::createUnitTexture2D(glm::vec3{1.0f, 1.0f, 1.0f});

  // vsync off
  glfwSwapInterval(0);

  lightCubeShader = gpu::Shader{"light-cube.vs", "light-cube.fs"};

  gpu::Shader lightingShader{"lit-shadows.vs", "lit-shadows.fs"};

  gpu::Shader pointShadowsDepthShader{"point-shadows-depth.vs",
                                      "point-shadows-depth.fs",
                                      "point-shadows-depth.gs"};

  lightingShader.setInt("diffuse_texture0", 0);
  lightingShader.setInt("specular_texture0", 1);
  lightingShader.setInt("normalMap", 2);

  for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i) {
    lightingShader.setInt("pointLightShadowMaps[" + std::to_string(i) + "]",
                          3 + i);
  }

  // point lights

  {
    {
      PointLight light;
      light.position = glm::vec3{0.0f, 1.0f, 0.0f};
      light.ambient = glm::vec3{0.01f};
      light.diffuse = glm::vec3{0.2, 0.2, 0.2};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }
    {
      PointLight light;
      light.position = glm::vec3{-1.8f, -0.5f, 1.7f};
      light.ambient = glm::vec3{0.01f};
      light.diffuse = glm::vec3{0.2, 0.2, 0.2};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }
    {
      PointLight light;
      light.position = glm::vec3{1.4f, -0.3f, -1.9f};
      light.ambient = glm::vec3{0.01f};
      light.diffuse = glm::vec3{0.2, 0.2, 0.2};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }
    {
      PointLight light;
      light.position = glm::vec3{1.7f, 0.3f, 1.5f};
      light.ambient = glm::vec3{0.01f};
      light.diffuse = glm::vec3{0.2, 0.2, 0.2};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }

    for (unsigned int i = 0; i < pointLights.size(); ++i) {
      pointLights[i].constantAtt = 1.0f;
      pointLights[i].linearAtt = 0.09f;
      pointLights[i].quadraticAtt = 0.032f;
    }
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  gpu::framebuffer::setClearColor(0.1f, 0.1f, 0.1f);

  while (!glfwWindowShouldClose(window)) {

    currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastTime;

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

    lastTime = currentTime;

    // input
    process_input(window);

    // first pass - generate shadows
    {
      float fovy = glm::radians(90.0f);
      float zNear = 0.1f;
      float zFar = 25.0f;

      pointShadowsDepthShader.setInt("nPointLights", nActiveLights);

      pointShadowsDepthShader.setFloat("zNear", zNear);
      pointShadowsDepthShader.setFloat("zFar", zFar);

      for (size_t i = 0; i < nActiveLights; ++i) {

        depthMapOmniFramebuffers[i].bind();

        {
          using namespace gpu::framebuffer;

          setViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
          clear(ClearFlagBits::DEPTH_BIT);
        }

        const PointLight &pointLight = pointLights[i];

        glm::mat4 projection =
            glm::perspective(fovy, shadowAspect, zNear, zFar);

        pointShadowsDepthShader.setVec3("lightPos", pointLight.position);

        // right face (+x)
        pointShadowsDepthShader.setMat4(
            "shadowMatrices[0]",
            projection *
                glm::lookAt(pointLight.position,
                            pointLight.position + glm::vec3{1.0f, 0.0f, 0.0f},
                            glm::vec3{0.0f, -1.0f, 0.0f}));

        // left face (-x)
        pointShadowsDepthShader.setMat4(
            "shadowMatrices[1]",
            projection *
                glm::lookAt(pointLight.position,
                            pointLight.position + glm::vec3{-1.0f, 0.0f, 0.0f},
                            glm::vec3{0.0f, -1.0f, 0.0f}));

        // up face (+y)
        pointShadowsDepthShader.setMat4(
            "shadowMatrices[2]",
            projection *
                glm::lookAt(pointLight.position,
                            pointLight.position + glm::vec3{0.0f, 1.0f, 0.0f},
                            glm::vec3{0.0f, 0.0f, 1.0f}));

        // down face (-y)
        pointShadowsDepthShader.setMat4(
            "shadowMatrices[3]",
            projection *
                glm::lookAt(pointLight.position,
                            pointLight.position + glm::vec3{0.0f, -1.0f, 0.0f},
                            glm::vec3{0.0f, 0.0f, -1.0f}));

        // front face (+z)
        pointShadowsDepthShader.setMat4(
            "shadowMatrices[4]",
            projection *
                glm::lookAt(pointLight.position,
                            pointLight.position + glm::vec3{0.0f, 0.0f, 1.0f},
                            glm::vec3{0.0f, -1.0f, 0.0f}));

        // back face (-z)
        pointShadowsDepthShader.setMat4(
            "shadowMatrices[5]",
            projection *
                glm::lookAt(pointLight.position,
                            pointLight.position + glm::vec3{0.0f, 0.0f, -1.0f},
                            glm::vec3{0.0f, -1.0f, 0.0f}));

        drawScene(pointShadowsDepthShader);
      }
    }

    // draw scene normally (with shadow info from the shadow pass)
    {
      {
        using namespace gpu::framebuffer;

        bindDefault();

        setViewport(0, 0, WIDTH, HEIGHT);
        clear(ClearFlagBits::COLOR_BIT | ClearFlagBits::DEPTH_BIT);
      }

      // uniform buffers
      {
        camUniformBuffer.updateSubdata("cameraPosition", camera.getPosition());
        camUniformBuffer.updateSubdata("cameraView", camera.getViewMatrix());
        camUniformBuffer.updateSubdata("cameraProjection",
                                       camera.getProjectionMatrix());
      }

      // point lights
      {
        lightingShader.setInt("nPointLights", nActiveLights);

        for (size_t i = 0; i < nActiveLights; ++i) {

          PointLight &point = pointLights[i];
          std::string prefix = "pointLights[" + std::to_string(i) + "]";

          lightingShader.setFloat(prefix + ".zNear", 0.1f);
          lightingShader.setFloat(prefix + ".zFar", 25.0f);

          lightingShader.setVec3(prefix + ".position", point.position);

          lightingShader.setVec3(prefix + ".ambient", point.ambient);
          lightingShader.setVec3(prefix + ".diffuse", point.diffuse);
          lightingShader.setVec3(prefix + ".specular", point.specular);

          lightingShader.setFloat(prefix + ".constantAtt", point.constantAtt);
          lightingShader.setFloat(prefix + ".linearAtt", point.linearAtt);
          lightingShader.setFloat(prefix + ".quadraticAtt", point.quadraticAtt);

          lightingShader.setInt(
              "pointLightShadowMaps[" + std::to_string(i) + "]", 3 + i);

          glBindTextureUnit(3 + i,
                            depthMapOmniFramebuffers[i].getDepthAttachmentID());
        }
      }

      drawScene(lightingShader);
      drawLightCubes();
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  camUniformBuffer.destroy();

  lightingShader.destroy();

  glfwTerminate();

  return 0;
}

void drawLightCubes() {

  lightCubeShader.use();

  for (size_t i = 0; i < nActiveLights; ++i) {

    glm::mat4 model{1.0f};
    model = glm::translate(model, pointLights[i].position);
    model = glm::scale(model, glm::vec3{0.1f});

    lightCubeShader.setMat4("model", model);
    lightCubeShader.setVec3("lightColor",
                            glm::normalize(pointLights[i].diffuse));

    cubeMesh.draw(lightCubeShader);
  }

  glUseProgram(0);
  glBindVertexArray(0);
}

void drawScene(const gpu::Shader &shader) {

  shader.use();

  glBindTextureUnit(0, wallDiffuseTexture.getID());
  glBindTextureUnit(1, whiteUnitTexture.getID());
  glBindTextureUnit(2, wallNormalTexture.getID());

  // cube
  {
    glm::mat4 model{1.0f};
    model = glm::rotate(model, -currentTime,
                        glm::normalize(glm::vec3{1.0f, 0.0f, 1.0f}));

    model = glm::scale(model, glm::vec3{1.0f, 1.0f, 0.01f});

    shader.setMat4("model", model);

    cubeMesh.draw(shader);
  }

  glUseProgram(0);
  glBindVertexArray(0);
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
    nActiveLights = 1;
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
    nActiveLights = 2;
  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
    nActiveLights = 3;
  } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
    nActiveLights = 4;
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
