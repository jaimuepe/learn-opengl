#ifndef SHADER_H
#define SHADER_H

#include "resources.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {

public:
  Shader(const char *vertexFile, const char *fragmentFile) {

    // retrieve the vertex/fragment source from filePath

    std::string vertexCode;
    std::string fragmentCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {

      // open files
      vShaderFile.open(getShaderPath(vertexFile));
      fShaderFile.open(getShaderPath(fragmentFile));

      std::stringstream vShaderStream;
      std::stringstream fShaderStream;

      // read file buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();

      // close handlers
      vShaderFile.close();
      fShaderFile.close();

      // convert into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();

    } catch (std::fstream::failure &e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULY_READ" << std::endl
                << e.what() << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // compile shaders

    GLuint vertexShaderId;
    GLuint fragmentShaderId;

    createShader(vShaderCode, GL_VERTEX_SHADER, vertexShaderId);
    createShader(fShaderCode, GL_FRAGMENT_SHADER, fragmentShaderId);

    createProgram(vertexShaderId, fragmentShaderId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
  }

  void use() { glUseProgram(m_ID); }

  void setInt(const std::string &name, int value) const {
    glProgramUniform1i(m_ID, glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void setFloat(const std::string &name, float value) const {
    glProgramUniform1f(m_ID, glGetUniformLocation(m_ID, name.c_str()), value);
  }

  void setFloat4(const std::string &name, float x, float y, float z,
                 float w) const {
    glProgramUniform4f(m_ID, glGetUniformLocation(m_ID, name.c_str()), x, y, z,
                       w);
  }

  void setMat4(const std::string &name, const GLfloat *value) {
    glProgramUniformMatrix4fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1,
                              GL_FALSE, value);
  }

  void setMat4(const std::string &name, const glm::mat4 &value) {
    glProgramUniformMatrix4fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1,
                              GL_FALSE, glm::value_ptr(value));
  }

private:
  GLuint m_ID;

  bool createShader(const char *shaderCode, GLenum type, GLuint &shaderId) {

    shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &shaderCode, nullptr);

    glCompileShader(shaderId);

    int success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success) {

      char infoLog[512];
      glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);

      std::cout << "ERROR::SHADER::"
                << (type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "VERTEX")
                << "::COMPILATION_FAILED\n"
                << infoLog << std::endl;

      return false;
    }

    return true;
  }

  bool createProgram(GLuint vertexShaderId, GLuint fragmentShaderId) {

    m_ID = glCreateProgram();

    glAttachShader(m_ID, vertexShaderId);
    glAttachShader(m_ID, fragmentShaderId);

    glLinkProgram(m_ID);

    int success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    if (!success) {

      char infoLog[512];
      glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);

      std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

      return false;
    }

    return true;
  }
};

#endif // SHADER_H
