#ifndef SHADER_H
#define SHADER_H

#include "resources.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

class Shader {

public:
  Shader() : m_ID(0){};

  Shader(const std::string &vertexFile, const std::string &fragmentFile,
         const std::optional<std::string> &geometryFile = {}) {

    std::string vertexCode = readShaderFile(vertexFile);
    std::string fragmentCode = readShaderFile(fragmentFile);

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // compile shaders

    GLuint vertexShaderId;
    GLuint fragmentShaderId;
    std::optional<GLuint> geometryShaderId;

    createShader(vShaderCode, GL_VERTEX_SHADER, vertexShaderId);
    createShader(fShaderCode, GL_FRAGMENT_SHADER, fragmentShaderId);

    if (geometryFile.has_value()) {

      std::string geometryCode = readShaderFile(geometryFile.value());
      const char *gShaderCode = geometryCode.c_str();

      GLuint gId;
      createShader(gShaderCode, GL_GEOMETRY_SHADER, gId);

      geometryShaderId = std::optional<GLuint>{gId};
    }

    createProgram(vertexShaderId, fragmentShaderId, geometryShaderId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    if (geometryShaderId.has_value()) {
      glDeleteShader(geometryShaderId.value());
    }
  }

  inline GLuint getID() const { return m_ID; }

  inline void use() const { glUseProgram(m_ID); }

  inline void setInt(const std::string &name, int value) const {
    glProgramUniform1i(m_ID, getUniformLocation(name), value);
  }

  inline void setFloat(const std::string &name, float value) const {
    glProgramUniform1f(m_ID, getUniformLocation(name), value);
  }

  inline void setVec2(const std::string &name, float x, float y) const {
    glProgramUniform2f(m_ID, getUniformLocation(name), x, y);
  }

  inline void setVec2(const std::string &name, const glm::vec2 &vec) const {
    glProgramUniform2f(m_ID, getUniformLocation(name), vec.x, vec.y);
  }

  inline void setVec3(const std::string &name, float x, float y,
                      float z) const {
    glProgramUniform3f(m_ID, getUniformLocation(name), x, y, z);
  }

  inline void setVec3(const std::string &name, const glm::vec3 &vec) const {
    glProgramUniform3f(m_ID, getUniformLocation(name), vec.x, vec.y, vec.z);
  }

  inline void setVec4(const std::string &name, float x, float y, float z,
                      float w) const {
    glProgramUniform4f(m_ID, getUniformLocation(name), x, y, z, w);
  }

  inline void setMat4(const std::string &name, const glm::mat4 &value) const {
    glProgramUniformMatrix4fv(m_ID, getUniformLocation(name), 1, GL_FALSE,
                              glm::value_ptr(value));
  }

private:
  GLuint m_ID;

  std::string readShaderFile(const std::string &filename) const {

    std::string code;

    std::ifstream shaderFile;

    // ensure ifstream objects can throw exceptions
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {

      // open files
      shaderFile.open(getShaderPath(filename));

      std::stringstream shaderStream;

      // read file buffer contents into streams
      shaderStream << shaderFile.rdbuf();

      // close handlers
      shaderFile.close();

      // convert into string
      code = shaderStream.str();

    } catch (std::fstream::failure &e) {

      std::stringstream ss;
      ss << "Error reading shader file " << filename << ":\n" << e.what();

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
                           GL_DEBUG_SEVERITY_HIGH, message.length(),
                           message.c_str());
    }

    return code;
  }

  bool createShader(const char *shaderCode, GLenum type, GLuint &shaderId) {

    shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &shaderCode, nullptr);

    glCompileShader(shaderId);

    int success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success) {

      char infoLog[512];
      glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);

      std::string strType;
      switch (type) {
      case GL_VERTEX_SHADER:
        strType = "vertex";
        break;
      case GL_GEOMETRY_SHADER:
        strType = "geometry";
        break;
      case GL_FRAGMENT_SHADER:
        strType = "fragment";
        break;
      default:
        strType = "???";
      }

      std::stringstream ss;
      ss << "Error compiling " << strType << " shader:\n" << infoLog;

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           shaderId, GL_DEBUG_SEVERITY_HIGH, message.length(),
                           message.c_str());

      return false;
    }

    return true;
  }

  bool createProgram(GLuint vertexShaderId, GLuint fragmentShaderId,
                     std::optional<GLuint> geometryShaderId) {

    m_ID = glCreateProgram();

    glAttachShader(m_ID, vertexShaderId);
    glAttachShader(m_ID, fragmentShaderId);

    if (geometryShaderId.has_value()) {
      glAttachShader(m_ID, geometryShaderId.value());
    }

    glLinkProgram(m_ID);

    int success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    if (!success) {

      char infoLog[512];
      glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);

      std::stringstream ss;
      ss << "Error linking program " << m_ID << ":\n" << infoLog;

      std::string message = ss.str();

      glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                           m_ID, GL_DEBUG_SEVERITY_HIGH, message.length(),
                           message.c_str());

      return false;
    }

    return true;
  }

  int getUniformLocation(const std::string &name) const {

    int loc = glGetUniformLocation(m_ID, name.c_str());

    /*
        // this method creates a lot of errors when the glsl compiler decides to
        // remove a variable. for now i'm going to disable it

        if (loc == -1) {

          std::stringstream ss;
          ss << "Undefined uniform in shader " << m_ID << ": " << name;

          std::string message = ss.str();

          glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR,
                               m_ID, GL_DEBUG_SEVERITY_MEDIUM, message.length(),
                               message.c_str());
        }
    */
    return loc;
  }
};

#endif // SHADER_H
