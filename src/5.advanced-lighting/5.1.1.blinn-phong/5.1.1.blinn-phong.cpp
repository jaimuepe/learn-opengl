
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

constexpr int WIDTH = 1200;
constexpr int HEIGHT = 900;

float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

float lastMouseX = static_cast<float>(WIDTH / 2);
float lastMouseY = static_cast<float>(HEIGHT / 2);

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
  glfwWindowHint(GLFW_SAMPLES, 4);

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

      // back face
       0.5f, -0.5f, -0.5f, // bottom-left
      -0.5f, -0.5f, -0.5f, // top-left
       0.5f,  0.5f, -0.5f, // bottom-right
       0.5f,  0.5f, -0.5f, // bottom-right
      -0.5f, -0.5f, -0.5f, // top-left
      -0.5f,  0.5f, -0.5f, // top-right

      // front face
      -0.5f, -0.5f,  0.5f, // bottom-left
       0.5f, -0.5f,  0.5f, // bottom-right
      -0.5f,  0.5f,  0.5f, // top-left
      -0.5f,  0.5f,  0.5f, // top-left
       0.5f, -0.5f,  0.5f, // bottom-right
       0.5f,  0.5f,  0.5f, // top-right

      // left face
      -0.5f, -0.5f, -0.5f, // bottom-left
      -0.5f, -0.5f,  0.5f, // bottom-right
      -0.5f,  0.5f, -0.5f, // top-left
      -0.5f,  0.5f, -0.5f, // top-left
      -0.5f, -0.5f,  0.5f, // bottom-right
      -0.5f,  0.5f,  0.5f, // top-right

      // right face
       0.5f, -0.5f,  0.5f, // bottom-left
       0.5f, -0.5f, -0.5f, // bottom-right
       0.5f,  0.5f,  0.5f, // top-left
       0.5f,  0.5f,  0.5f, // top-left
       0.5f, -0.5f, -0.5f, // bottom-right
       0.5f,  0.5f, -0.5f, // top-right

      // bottom face
      -0.5f, -0.5f, -0.5f, // bottom-left
       0.5f, -0.5f, -0.5f, // bottom-right
      -0.5f, -0.5f,  0.5f, // top-left
      -0.5f, -0.5f,  0.5f, // top-left
       0.5f, -0.5f, -0.5f, // bottom-right
       0.5f, -0.5f,  0.5f, // top-right

      // top-face
      -0.5f,  0.5f,  0.5f, // bottom-left
       0.5f,  0.5f,  0.5f, // bottom-right
      -0.5f,  0.5f, -0.5f, // top-left
      -0.5f,  0.5f, -0.5f, // top-left
       0.5f,  0.5f,  0.5f, // bottom-right
       0.5f,  0.5f, -0.5f, // top-right
  };

  float planeVertices[] = {
      // vposition            // normals           // texcoords
      -10.0f, 0.0f, -10.0f,   0.0f, 1.0f, 0.0f,    0.0f, 10.0f, // top-left
      -10.0f, 0.0f,  10.0f,   0.0f, 1.0f, 0.0f,    0.0f,  0.0f, // bottom left
       10.0f, 0.0f, -10.0f,   0.0f, 1.0f, 0.0f,   10.0f, 10.0f, // top-right

       10.0f, 0.0f, -10.0f,   0.0f, 1.0f, 0.0f,   10.0f, 10.0f, // top-right
      -10.0f, 0.0f,  10.0f,   0.0f, 1.0f, 0.0f,    0.0f,  0.0f, // bottom-left
       10.0f, 0.0f,  10.0f,   0.0f, 1.0f, 0.0f,   10.0f,  0.0f  // bottom-right
  };

  // clang-format on

  // uniform buffers

  GLuint camUbo;
  {
    // view matrix, perspective matrix
    glCreateBuffers(1, &camUbo);
    glNamedBufferData(camUbo, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, camUbo);
  }

  // cube light
  GLuint cubeLightVBO;
  GLuint cubeLightVAO;
  {
    glCreateBuffers(1, &cubeLightVBO);
    glNamedBufferData(cubeLightVBO, sizeof(cubeVertices), cubeVertices,
                      GL_STATIC_DRAW);

    glCreateVertexArrays(1, &cubeLightVAO);

    // positions

    glEnableVertexArrayAttrib(cubeLightVAO, 0);
    glVertexArrayAttribFormat(cubeLightVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(cubeLightVAO, 0, cubeLightVBO, 0,
                              3 * sizeof(float));
    glVertexArrayAttribBinding(cubeLightVAO, 0, 0);
  }
  // quad

  GLuint quadVBO;
  GLuint quadVAO;
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

  GLuint woodTex;
  {
    stbi_set_flip_vertically_on_load(true);

    glCreateTextures(GL_TEXTURE_2D, 1, &woodTex);

    int width;
    int height;
    int nrChannels;

    std::string texPath = getTexturePath("wood.png");

    unsigned char *data =
        stbi_load(texPath.c_str(), &width, &height, &nrChannels, 0);

    GLenum internalFormat = nrChannels == 3 ? GL_RGB8 : GL_RGBA8;
    GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;

    glTextureParameteri(woodTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(woodTex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(woodTex, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);

    glTextureParameteri(woodTex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureStorage2D(woodTex, 1, internalFormat, width, height);
    glTextureSubImage2D(woodTex, 0, 0, 0, width, height, format,
                        GL_UNSIGNED_BYTE, data);

    glGenerateTextureMipmap(woodTex);

    stbi_image_free(data);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_CULL_FACE);

  // vsync off
  glfwSwapInterval(0);

  std::vector<PointLight> lights{4};
  {
    lights[0].position = glm::vec3{-2.0f, 0.0f, -2.0f};
    lights[0].ambient = glm::vec3{0.05f, 0.05f, 0.05f};
    lights[0].diffuse = glm::normalize(glm::vec3{1.0f, 0.2f, 0.2f});
    lights[0].specular = glm::vec3{0.3f, 0.3f, 0.3f};

    lights[1].position = glm::vec3{-2.0f, 0.0f, 2.0f};
    lights[1].ambient = glm::vec3{0.05f, 0.05f, 0.05f};
    lights[1].diffuse = glm::normalize(glm::vec3{0.1f, 1.0f, 0.1f});
    lights[1].specular = glm::vec3{0.3f, 0.3f, 0.3f};

    lights[2].position = glm::vec3{2.0f, 0.0f, -2.0f};
    lights[2].ambient = glm::vec3{0.05f, 0.05f, 0.05f};
    lights[2].diffuse = glm::normalize(glm::vec3{1.0f, 1.0f, 0.2f});
    lights[2].specular = glm::vec3{0.3f, 0.3f, 0.3f};

    lights[3].position = glm::vec3{2.0f, 0.0f, 2.0f};
    lights[3].ambient = glm::vec3{0.05f, 0.05f, 0.05f};
    lights[3].diffuse = glm::normalize(glm::vec3{0.1f, 0.1f, 1.0f});
    lights[3].specular = glm::vec3{0.3f, 0.3f, 0.3f};

    for (int i = 0; i < lights.size(); ++i) {
      lights[i].constantAtt = 1.0f;
      lights[i].linearAtt = 0.09f;
      lights[i].quadraticAtt = 0.032f;
    }
  }

  Shader lightingShader("advanced-lighting.vs", "advanced-lighting.fs");
  lightingShader.setInt("diffuse_texture0", 0);

  Shader cubeLightShader("cube-light.vs", "cube-light.fs");

  glBindTextureUnit(0, woodTex);

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

    // uniform buffers
    {
      glNamedBufferSubData(camUbo, 0, sizeof(glm::mat4), glm::value_ptr(view));
      glNamedBufferSubData(camUbo, sizeof(glm::mat4), sizeof(glm::mat4),
                           glm::value_ptr(projection));
    }

    glBindVertexArray(quadVAO);

    lightingShader.use();

    // light
    {
      lightingShader.setInt("nPointLights", lights.size());

      for (int i = 0; i < lights.size(); ++i) {

        const PointLight &light = lights[i];

        // draw light
        {
          glBindVertexArray(cubeLightVAO);

          cubeLightShader.use();

          glm::mat4 model = glm::mat4{1.0};
          model = glm::translate(model, light.position);
          model = glm::scale(model, glm::vec3{0.1f});

          cubeLightShader.setMat4("model", model);
          cubeLightShader.setVec3("lightColor", light.diffuse);

          glDrawArrays(GL_TRIANGLES, 0, 36);

          glBindVertexArray(0);
          glUseProgram(0);
        }

        // set light data in main shader
        {
          std::string arrayLoc = "pointLights[" + std::to_string(i) + "]";

          glm::vec3 lightPosViewSpace =
              glm::vec3{view * glm::vec4{light.position, 1.0}};

          lightingShader.setVec3(arrayLoc + ".position", lightPosViewSpace);

          lightingShader.setVec3(arrayLoc + ".ambient", light.ambient);
          lightingShader.setVec3(arrayLoc + ".diffuse", light.diffuse);
          lightingShader.setVec3(arrayLoc + ".specular", light.specular);

          lightingShader.setFloat(arrayLoc + ".constantAtt", light.constantAtt);
          lightingShader.setFloat(arrayLoc + ".linearAtt", light.linearAtt);
          lightingShader.setFloat(arrayLoc + ".quadraticAtt",
                                  light.quadraticAtt);
        }
      }
    }

    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3{0.0f, -0.5f, 0.0f});

    lightingShader.setMat4("model", model);

    glBindVertexArray(quadVAO);
    lightingShader.use();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glUseProgram(0);

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &quadVAO);

  glDeleteBuffers(1, &quadVBO);

  glDeleteBuffers(1, &camUbo);

  glDeleteProgram(lightingShader.getID());
  glDeleteTextures(1, &woodTex);

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
