
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

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

constexpr int SHADOW_WIDTH = 1024;
constexpr int SHADOW_HEIGHT = 1024;

float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

float lastMouseX = static_cast<float>(WIDTH) * 0.5f;
float lastMouseY = static_cast<float>(HEIGHT) * 0.5f;

GLuint quadVBO;
GLuint quadVAO;

GLuint cubeVAO;
GLuint cubeVBO;

GLuint screenQuadVBO;
GLuint screenQuadVAO;

GLuint containerDiffTex;
GLuint containerSpecTex;

GLuint woodTex;
GLuint whiteTex;

FlyCamera camera{glm::vec3{0.0f, 0.0f, 3.0f}, glm::radians(45.0f), aspect, 0.1f,
                 100.0f};

void drawScene(const Shader &shader);
void fillLightInfo(const Shader &shader);

GLuint createTexture(const std::string &texName);

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

  GLuint depthMapFBO;
  GLuint depthTexture;
  {
    glCreateFramebuffers(1, &depthMapFBO);
    glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture);

    glTextureStorage2D(depthTexture, 1, GL_DEPTH_COMPONENT16, SHADOW_WIDTH,
                       SHADOW_HEIGHT);

    glNamedFramebufferTexture(depthMapFBO, GL_DEPTH_ATTACHMENT, depthTexture,
                              0);

    if (glCheckNamedFramebufferStatus(depthMapFBO, GL_FRAMEBUFFER) !=
        GL_FRAMEBUFFER_COMPLETE) {

      std::stringstream ss;
      ss << "Framebuffer is not complete!";

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           depthMapFBO, GL_DEBUG_SEVERITY_MEDIUM,
                           message.length(), message.c_str());
    }
  }

  // uniform buffers

  GLuint camUbo;
  {
    // view matrix, perspective matrix
    glCreateBuffers(1, &camUbo);
    glNamedBufferData(camUbo, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

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

  containerDiffTex = createTexture("container2.png");
  containerSpecTex = createTexture("container2_specular.png");

  woodTex = createTexture("wood.png");

  glCreateTextures(GL_TEXTURE_2D, 1, &whiteTex);

  float data[] = {0.3f, 0.3f, 0.3f};
  glTextureStorage2D(whiteTex, 1, GL_RGB8, 1, 1);
  glTextureSubImage2D(whiteTex, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, &data);

  // vsync off
  glfwSwapInterval(0);

  Shader lightingShader("lit.vs", "lit.fs");

  Shader shadowMappingDepthShader("shadow-mapping-depth.vs",
                                  "shadow-mapping-depth.fs");

  Shader debugScreenShader("debug-quad-depth.vs", "debug-quad-depth.fs");

  lightingShader.setInt("diffuse_texture0", 0);
  lightingShader.setInt("specular_texture0", 1);

  debugScreenShader.setFloat("far", camera.getZFar());
  debugScreenShader.setFloat("near", camera.getZNear());

  debugScreenShader.setInt("depthMap", 0);

  // dirLight params

  glm::vec3 lightDir = glm::vec3{0.2f, -0.4f, 0.1f};

  lightingShader.setVec3("dirLight.ambient", 0.02f, 0.02f, 0.02f);
  lightingShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
  lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

  // no point lights for now
  lightingShader.setInt("nPointLights", 0);

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
      // a directional light doesn't really have a position, but we need one for
      // the algorithm so we'll pick a point and a ray that crosses the origin

      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

      glEnable(GL_DEPTH_TEST);
      glClear(GL_DEPTH_BUFFER_BIT);

      glm::vec3 lightPos = -10.0f * lightDir;

      glm::mat4 view = glm::lookAt(lightPos, glm::vec3{0.0f, 0.0f, 0.0f},
                                   glm::vec3{0.0f, 1.0f, 0.0f});

      // ortho, since a directional light has no perspective distoriton
      glm::mat4 projection =
          glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);

      glm::mat4 lightSpaceMatrix = projection * view;

      shadowMappingDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

      drawScene(shadowMappingDepthShader);
    }

    // second pass
    // visualize shadow depth buffer
    {
      glDisable(GL_DEPTH_TEST);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, WIDTH, HEIGHT);

      glClear(GL_COLOR_BUFFER_BIT);

      glBindVertexArray(screenQuadVAO);
      glBindTextureUnit(0, depthTexture);

      debugScreenShader.use();

      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // second pass
    // draw scene normally
    if (false) {

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glViewport(0, 0, WIDTH, HEIGHT);

      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      const glm::mat4 &view = camera.getViewMatrix();
      const glm::mat4 &projection = camera.getProjectionMatrix();

      // uniform buffers
      {
        glNamedBufferSubData(camUbo, 0, sizeof(glm::mat4),
                             glm::value_ptr(view));
        glNamedBufferSubData(camUbo, sizeof(glm::mat4), sizeof(glm::mat4),
                             glm::value_ptr(projection));
      }

      // dir light
      {
        const glm::mat4 invView = camera.getInverseViewMatrix();

        glm::vec3 lightDirCameraSpace =
            glm::vec3{glm::transpose(invView) * glm::vec4{lightDir, 0.0}};

        lightingShader.setVec3("dirLight.direction", lightDirCameraSpace);
      }

      drawScene(lightingShader);
    }

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteVertexArrays(1, &cubeVAO);

  glDeleteBuffers(1, &quadVBO);
  glDeleteBuffers(1, &cubeVBO);

  glDeleteBuffers(1, &camUbo);

  glDeleteProgram(lightingShader.getID());

  glDeleteTextures(1, &containerDiffTex);
  glDeleteTextures(1, &containerSpecTex);

  glfwTerminate();

  return 0;
}

GLuint createTexture(const std::string &texName) {

  GLuint texId;

  stbi_set_flip_vertically_on_load(true);

  glCreateTextures(GL_TEXTURE_2D, 1, &texId);

  int width;
  int height;
  int nrChannels;

  std::string texPath = getTexturePath(texName);

  unsigned char *data =
      stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

  if (data) {

    GLenum internalFormat = nrChannels == 3 ? GL_RGB8 : GL_RGBA8;
    GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;

    glTextureParameteri(texId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texId, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(texId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTextureParameteri(texId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(texId, 1, internalFormat, width, height);
    glTextureSubImage2D(texId, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE,
                        data);

    glGenerateTextureMipmap(texId);

    stbi_image_free(data);

    return texId;

  } else {

    std::stringstream ss;
    ss << "Could not create texture " << texPath;

    std::string message = ss.str();

    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                         texId, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                         message.c_str());

    return 0;
  }
}

void fillLightInfo(const Shader &shader) {}

void drawScene(const Shader &shader) {

  shader.use();

  // floor
  {
    glBindTextureUnit(0, woodTex);
    glBindTextureUnit(1, whiteTex);

    glBindVertexArray(quadVAO);

    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{0.0f, -0.5f, 0.0f});

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  // cubes
  {
    glBindTextureUnit(0, containerDiffTex);
    glBindTextureUnit(1, containerSpecTex);

    glBindVertexArray(cubeVAO);

    // 1
    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{0.0f, 1.5f, 0.0});
    model = glm::scale(model, glm::vec3{0.5f});
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2
    model = glm::mat4{1.0f};
    model = glm::translate(model, glm::vec3{2.0f, 0.0f, 1.0});
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
