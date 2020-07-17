
#include <glad\glad.h>

#include <GLFW\glfw3.h>

#include <iostream>

const char *vertexShaderSource = R"src(
    #version 450 core

    layout (location = 0) in vec3 aPos;

    void main() 
    {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)src";

const char *fragmentShaderSource = R"src(
    #version 450 core

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)src";

const char *fragmentShaderSource2 = R"src(
    #version 450 core

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
    }
)src";

void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main() {

  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);

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

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // clang-format off

  const float vertices[2][9] = {

      { 
        -0.25f, -0.25f, 0.0f,
        -0.5f,  0.25f, 0.0f,
        -0.75f, -0.25f, 0.0f
      },
      {
        0.75f, -0.25f, 0.0f,
        0.5f, 0.25f, 0.0f,
        0.25f, -0.25f, 0.0f
      }
  };

  // clang-format on

  GLuint VAOs[2];
  glCreateVertexArrays(2, VAOs);

  GLuint VBOs[2];
  glCreateBuffers(2, VBOs);

  GLuint shaderPrograms[2];

  const char *vertexShaderSources[2] = {vertexShaderSource, vertexShaderSource};

  const char *fragmentShaderSources[2] = {fragmentShaderSource,
                                          fragmentShaderSource2};

  for (int i = 0; i < 2; ++i) {

    glNamedBufferData(VBOs[i], 9 * sizeof(float), vertices[i], GL_STATIC_DRAW);

    // enable attrib 0
    glEnableVertexArrayAttrib(VAOs[i], 0);

    // attrib 0 has 3 values of float size, no relative offset
    glVertexArrayAttribFormat(VAOs[i], 0, 3, GL_FLOAT, GL_FALSE, 0);

    // binding with VBO at index 0, no offset, stride = 3 float (12 bytes)
    glVertexArrayVertexBuffer(VAOs[i], 0, VBOs[i], 0, 3 * sizeof(float));

    // binding VBO at 0 with attrib at 0
    glVertexArrayAttribBinding(VAOs[i], 0, 0);

    glBindVertexArray(0);

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSources[i], nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {

      glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);

      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSources[i], nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {

      glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);

      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;
    }

    shaderPrograms[i] = glCreateProgram();
    glAttachShader(shaderPrograms[i], vertexShader);
    glAttachShader(shaderPrograms[i], fragmentShader);
    glLinkProgram(shaderPrograms[i]);

    glGetProgramiv(shaderPrograms[i], GL_LINK_STATUS, &success);

    if (!success) {

      glGetProgramInfoLog(shaderPrograms[i], 512, nullptr, infoLog);

      std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  while (!glfwWindowShouldClose(window)) {

    // input
    process_input(window);

    // rendering
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < 2; ++i) {

      glUseProgram(shaderPrograms[i]);

      glBindVertexArray(VAOs[i]);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // sysevents and buffer swaping
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}