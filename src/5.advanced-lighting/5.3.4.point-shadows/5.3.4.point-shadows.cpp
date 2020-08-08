
#include "flycamera.h"
#include "framebuffer.h"
#include "model.h"
#include "pointlight.h"
#include "renderbuffer.h"
#include "shader.h"
#include "texture2d.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

float cameraSpeed = 3.0f;

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

Model *suzzane;
Model *backpack;

GLuint quadVBO;
GLuint quadVAO;

GLuint cubeVAO;
GLuint cubeVBO;

GLuint screenQuadVBO;
GLuint screenQuadVAO;

gpu::Texture2D containerDiffTex;
gpu::Texture2D containerSpecTex;

gpu::Texture2D woodTex;

GLuint whiteTex03;
GLuint whiteTex10;

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f), aspect, 0.1f,
                 100.0f};

std::vector<PointLight> pointLights;

Shader lightCubeShader;

void drawLightCubes();
void drawScene(const Shader &shader);

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
    
      // position             // normals            // texcoords
      
      // back face
       0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f, // bottom-left
      -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f, // top-left
       0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f, // bottom-right
       0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f, // bottom-right
      -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f, // top-left
      -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f, // top-right

      // front face
      -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f, // bottom-left
       0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f, // bottom-right
      -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f, // top-left
      -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f, // top-left
       0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f, // bottom-right
       0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f, // top-right

      // left face
      -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f, // bottom-left
      -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
      -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
      -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
      -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
      -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, // top-right

      // right face
       0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f, // bottom-left
       0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
       0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
       0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f, // top-left
       0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f, // bottom-right
       0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f, // top-right

      // bottom face
      -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f, // bottom-left
       0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
      -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f, // top-left
      -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f, // top-left
       0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
       0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f, // top-right

      // top-face
      -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f, // bottom-left
       0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
      -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f, // top-left
      -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f, // top-left
       0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f, // bottom-right
       0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f, // top-right
  };

  float planeVertices[] = {
      // vposition            // normals           // texcoords
      -25.0f, 0.0f, -25.0f,   0.0f, 1.0f, 0.0f,    0.0f, 25.0f, // top-left
      -25.0f, 0.0f,  25.0f,   0.0f, 1.0f, 0.0f,    0.0f,  0.0f, // bottom left
       25.0f, 0.0f, -25.0f,   0.0f, 1.0f, 0.0f,   25.0f, 25.0f, // top-right

       25.0f, 0.0f, -25.0f,   0.0f, 1.0f, 0.0f,   25.0f, 25.0f, // top-right
      -25.0f, 0.0f,  25.0f,   0.0f, 1.0f, 0.0f,    0.0f,  0.0f, // bottom-left
       25.0f, 0.0f,  25.0f,   0.0f, 1.0f, 0.0f,   25.0f,  0.0f  // bottom-right
  };

  float screenQuadVertices[] = {

      // pos(x, y)  // texcoords
      -1.0f,  1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 1.0f, 0.0f,

       1.0f, -1.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f, 1.0f
  };

  // clang-format on

  // shadow mapping

  // directional

  gpu::Framebuffer depthMapFramebuffer;
  gpu::Texture2D depthTexture{SHADOW_WIDTH, SHADOW_HEIGHT,
                              GL_DEPTH_COMPONENT16};

  depthTexture.setWrapST(gpu::texture::Wrap::CLAMP_TO_BORDER);
  depthTexture.setMinMagFilter(gpu::texture::Filter::NEAREST);
  depthTexture.setBorderColor(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

  depthMapFramebuffer.attachDepth(depthTexture);
  depthMapFramebuffer.checkStatus();

  // omni

  GLuint depthMapOmniFBOs[MAX_POINT_LIGHTS];
  GLuint depthCubemaps[MAX_POINT_LIGHTS];

  {
    for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i) {

      GLuint depthMapOmniFBO;
      GLuint depthCubemap;

      glCreateFramebuffers(1, &depthMapOmniFBO);

      glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &depthCubemap);
      glTextureStorage2D(depthCubemap, 1, GL_DEPTH_COMPONENT16, SHADOW_WIDTH,
                         SHADOW_HEIGHT);

      glTextureParameteri(depthCubemap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(depthCubemap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTextureParameteri(depthCubemap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTextureParameteri(depthCubemap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTextureParameteri(depthCubemap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glNamedFramebufferTexture(depthMapOmniFBO, GL_DEPTH_ATTACHMENT,
                                depthCubemap, 0);

      if (glCheckNamedFramebufferStatus(depthMapOmniFBO, GL_FRAMEBUFFER) !=
          GL_FRAMEBUFFER_COMPLETE) {

        std::stringstream ss;
        ss << "Framebuffer is not complete!";

        std::string message = ss.str();

        glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                             depthMapOmniFBO, GL_DEBUG_SEVERITY_MEDIUM,
                             message.length(), message.c_str());
      }

      depthMapOmniFBOs[i] = depthMapOmniFBO;
      depthCubemaps[i] = depthCubemap;
    }
  }

  // uniform buffers

  GLuint camUbo;
  {
    // camera position, view matrix, perspective matrix
    glCreateBuffers(1, &camUbo);
    glNamedBufferData(camUbo, sizeof(glm::vec4) + 2 * sizeof(glm::mat4),
                      nullptr, GL_STATIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camUbo);
  }

  // cubes
  {
    glCreateBuffers(1, &cubeVBO);
    glNamedBufferData(cubeVBO, sizeof(cubeVertices), cubeVertices,
                      GL_STATIC_DRAW);

    glCreateVertexArrays(1, &cubeVAO);

    // positions

    glEnableVertexArrayAttrib(cubeVAO, 0);
    glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(cubeVAO, 0, cubeVBO, 0, 8 * sizeof(float));
    glVertexArrayAttribBinding(cubeVAO, 0, 0);

    // normals

    glEnableVertexArrayAttrib(cubeVAO, 1);
    glVertexArrayAttribFormat(cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(cubeVAO, 1, cubeVBO, 3 * sizeof(float),
                              8 * sizeof(float));
    glVertexArrayAttribBinding(cubeVAO, 1, 1);

    // texcoords

    glEnableVertexArrayAttrib(cubeVAO, 2);
    glVertexArrayAttribFormat(cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(cubeVAO, 2, cubeVBO, 6 * sizeof(float),
                              8 * sizeof(float));
    glVertexArrayAttribBinding(cubeVAO, 2, 2);
  }

  // quad
  {
    glCreateBuffers(1, &quadVBO);
    glNamedBufferData(quadVBO, sizeof(planeVertices), planeVertices,
                      GL_STATIC_DRAW);

    glCreateVertexArrays(1, &quadVAO);

    // positions
    glEnableVertexArrayAttrib(quadVAO, 0);
    glVertexArrayAttribFormat(quadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(quadVAO, 0, quadVBO, 0, 8 * sizeof(float));
    glVertexArrayAttribBinding(quadVAO, 0, 0);

    // normals
    glEnableVertexArrayAttrib(quadVAO, 1);
    glVertexArrayAttribFormat(quadVAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(quadVAO, 1, quadVBO, 3 * sizeof(float),
                              8 * sizeof(float));
    glVertexArrayAttribBinding(quadVAO, 1, 1);

    // texcoords
    glEnableVertexArrayAttrib(quadVAO, 2);
    glVertexArrayAttribFormat(quadVAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(quadVAO, 2, quadVBO, 6 * sizeof(float),
                              8 * sizeof(float));
    glVertexArrayAttribBinding(quadVAO, 2, 2);
  }

  // screen quad
  {
    glCreateBuffers(1, &screenQuadVBO);
    glNamedBufferData(screenQuadVBO, sizeof(screenQuadVertices),
                      screenQuadVertices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &screenQuadVAO);

    // positions
    glEnableVertexArrayAttrib(screenQuadVAO, 0);
    glVertexArrayAttribFormat(screenQuadVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(screenQuadVAO, 0, screenQuadVBO, 0,
                              4 * sizeof(float));
    glVertexArrayAttribBinding(screenQuadVAO, 0, 0);

    // texcoords
    glEnableVertexArrayAttrib(screenQuadVAO, 1);
    glVertexArrayAttribFormat(screenQuadVAO, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(screenQuadVAO, 1, screenQuadVBO,
                              2 * sizeof(float), 4 * sizeof(float));
    glVertexArrayAttribBinding(screenQuadVAO, 1, 1);
  }

  containerDiffTex = gpu::Texture2D{"container2.png"};
  containerSpecTex = gpu::Texture2D{"container2_specular.png"};

  woodTex = gpu::Texture2D{"wood.png"};

  glCreateTextures(GL_TEXTURE_2D, 1, &whiteTex03);
  // 1px x 1px, single color texture (useful for default values)
  {
    float data[] = {0.3f, 0.3f, 0.3f};
    glTextureStorage2D(whiteTex03, 1, GL_RGB8, 1, 1);
    glTextureSubImage2D(whiteTex03, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, &data);
  }

  glCreateTextures(GL_TEXTURE_2D, 1, &whiteTex10);
  // 1px x 1px, single color texture (useful for default values)
  {
    float data[] = {1.0f, 1.0f, 1.0f};
    glTextureStorage2D(whiteTex10, 1, GL_RGB8, 1, 1);
    glTextureSubImage2D(whiteTex10, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, &data);
  }

  // vsync off
  glfwSwapInterval(0);

  std::stringstream monkeyModelPath;
  monkeyModelPath << getModelPath("monkey") << separator << "monkey.obj";

  suzzane = new Model{monkeyModelPath.str()};

  lightCubeShader = Shader{"light-cube.vs", "light-cube.fs"};

  Shader lightingShader{"lit-shadows.vs", "lit-shadows.fs"};

  Shader shadowMappingDepthShader{"shadow-mapping-depth.vs",
                                  "shadow-mapping-depth.fs"};

  Shader pointShadowsDepthShader{"point-shadows-depth.vs",
                                 "point-shadows-depth.fs",
                                 "point-shadows-depth.gs"};

  lightingShader.setInt("diffuse_texture0", 0);
  lightingShader.setInt("specular_texture0", 1);

  lightingShader.setInt("dirLightShadowMap", 2);

  // point lights

  {
    {
      PointLight light;
      // light.position = glm::vec3{0.0f, 3.0f, 0.0f};
      light.position = glm::vec3{-1.0f, 3.0f, -1.0f};
      light.ambient = glm::vec3{0.0f};
      light.diffuse = glm::vec3{0.3f, 0.0f, 0.0f};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }
    {
      PointLight light;
      light.position = glm::vec3{-1.0f, 3.0f, 1.0f};
      light.ambient = glm::vec3{0.0f};
      light.diffuse = glm::vec3{0.1f, 0.1f, 0.1f};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }
    {
      PointLight light;
      light.position = glm::vec3{1.0f, 3.0f, -1.0f};
      light.ambient = glm::vec3{0.0f};
      light.diffuse = glm::vec3{0.1f, 0.1f, 0.1f};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }
    {
      PointLight light;
      light.position = glm::vec3{1.0f, 3.0f, 1.0f};
      light.ambient = glm::vec3{0.0f};
      light.diffuse = glm::vec3{0.1f, 0.1f, 0.1f};
      light.specular = glm::vec3{0.3f, 0.3f, 0.3f};
      pointLights.push_back(light);
    }

    for (unsigned int i = 0; i < pointLights.size(); ++i) {
      pointLights[i].constantAtt = 1.0f;
      pointLights[i].linearAtt = 0.09f;
      pointLights[i].quadraticAtt = 0.032f;
    }
  }

  // dirLight params

  glm::vec3 lightDir = glm::vec3{0.2f, -0.4f, 0.1f};

  lightingShader.setVec3("dirLight.ambient", 0.02f, 0.02f, 0.02f);
  lightingShader.setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
  lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

  // if the directional light is going to be static we only need to do this once
  {
    glm::vec3 lightPos = -10.0f * lightDir;

    glm::mat4 view = glm::lookAt(lightPos, glm::vec3{0.0f, 0.0f, 0.0f},
                                 glm::vec3{0.0f, 1.0f, 0.0f});

    // ortho, since a directional light has no perspective distoriton
    glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);

    glm::mat4 lightSpaceMatrix = projection * view;

    shadowMappingDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    lightingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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

    // first pass
    // calculate depth from the light's perspective
    {

      // dir light

      depthMapFramebuffer.bind();

      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      glClear(GL_DEPTH_BUFFER_BIT);

      drawScene(shadowMappingDepthShader);

      // point lights

      float fovy = glm::radians(90.0f);
      float zNear = 0.1f;
      float zFar = 25.0f;

      for (size_t i = 0; i < pointLights.size(); ++i) {

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapOmniFBOs[i]);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        const PointLight &pointLight = pointLights[i];

        glm::mat4 projection =
            glm::perspective(fovy, shadowAspect, zNear, zFar);

        pointShadowsDepthShader.setVec3("lightPos", pointLight.position);
        pointShadowsDepthShader.setFloat("zNear", zNear);
        pointShadowsDepthShader.setFloat("zFar", zFar);

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

    // second pass
    // draw scene normally (with shadow info from the previous pass)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glViewport(0, 0, WIDTH, HEIGHT);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const glm::mat4 &view = camera.getViewMatrix();
      const glm::mat4 &projection = camera.getProjectionMatrix();

      // uniform buffers
      {
        // camera pos
        glNamedBufferSubData(camUbo, 0, sizeof(glm::vec4),
                             glm::value_ptr(camera.getPosition()));
        // camera view
        glNamedBufferSubData(camUbo, sizeof(glm::vec4), sizeof(glm::mat4),
                             glm::value_ptr(view));
        // camera projection
        glNamedBufferSubData(camUbo, sizeof(glm::vec4) + sizeof(glm::mat4),
                             sizeof(glm::mat4), glm::value_ptr(projection));
      }

      // dir light
      { lightingShader.setVec3("dirLight.direction", lightDir); }

      // point lights
      {
        lightingShader.setInt("nPointLights", pointLights.size());

        for (size_t i = 0; i < pointLights.size(); ++i) {

          const PointLight &point = pointLights[i];
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

          glBindTextureUnit(3 + i, depthCubemaps[i]);
        }
      }

      glBindTextureUnit(2, depthTexture.getID());

      drawScene(lightingShader);
      drawLightCubes();
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  delete suzzane;

  depthMapFramebuffer.destroy();

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteVertexArrays(1, &cubeVAO);

  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &cubeVBO);

  glDeleteBuffers(1, &camUbo);

  glDeleteProgram(lightingShader.getID());

  containerDiffTex.destroy();
  containerSpecTex.destroy();

  glDeleteTextures(1, &whiteTex03);
  glDeleteTextures(1, &whiteTex10);

  glfwTerminate();

  return 0;
}

void drawLightCubes() {

  glBindVertexArray(cubeVAO);

  lightCubeShader.use();

  for (size_t i = 0; i < pointLights.size(); ++i) {

    glm::mat4 model{1.0f};
    model = glm::translate(model, pointLights[i].position);
    model = glm::scale(model, glm::vec3{0.1f});

    lightCubeShader.setMat4("model", model);
    lightCubeShader.setVec3("lightColor",
                            glm::normalize(pointLights[i].diffuse));

    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  glUseProgram(0);
  glBindVertexArray(0);
}

void drawScene(const Shader &shader) {

  shader.use();

  // floor
  {
    glBindTextureUnit(0, woodTex.getID());
    glBindTextureUnit(1, whiteTex03);

    glBindVertexArray(quadVAO);

    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{0.0f, -0.5f, 0.0f});

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  // cubes
  {
    glBindTextureUnit(0, containerDiffTex.getID());
    glBindTextureUnit(1, containerSpecTex.getID());

    glBindVertexArray(cubeVAO);

    // 1
    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{0.0f, 0.75f, 0.0});
    model = glm::scale(model, glm::vec3{0.3f});
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2
    model = glm::mat4{1.0f};
    model = glm::translate(model, glm::vec3{2.0f, -0.25f, 1.0});
    model = glm::scale(model, glm::vec3{0.5f});

    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 3
    model = glm::mat4{1.0f};
    model = glm::translate(model, glm::vec3{-1.0f, 0.0f, 2.0});
    model = glm::rotate(model, glm::radians(60.0f),
                        glm::normalize(glm::vec3{1.0, 0.0, 1.0}));
    model = glm::scale(model, glm::vec3{0.25});

    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  // monkey
  {
    glBindTextureUnit(0, whiteTex10);
    glBindTextureUnit(1, 0);

    glm::mat4 model = glm::mat4{1.0f};

    model = glm::translate(model, glm::vec3{-2.0f, 1.0f, -2.0f});
    model =
        glm::rotate(model, -glm::radians(90.0f), glm::vec3{0.0f, 1.0f, 0.0f});
    model =
        glm::rotate(model, -glm::radians(15.0f), glm::vec3{1.0f, 0.0f, 0.0f});

    shader.setMat4("model", model);
    suzzane->draw(shader);
  }

  // backpack
  {
    glm::mat4 model = glm::mat4{1.0f};
    model = glm::translate(model, glm::vec3{0.0f, 1.0f, 0.0f});
    model = glm::scale(model, glm::vec3{0.6f});

    shader.setMat4("model", model);

    // backpack->draw(shader);
  }

  glUseProgram(0);
  glBindVertexArray(0);
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
