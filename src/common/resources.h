
#ifndef MY_RESOURCES_H
#define MY_RESOURCES_H

#include "filesystem.h"

std::string getTexturePath(std::string texLocalPath) {
  return getResPath().append("textures").append(texLocalPath).string();
}

std::string getShaderPath(std::string shaderLocalPath) {
  return getExecPath().append(shaderLocalPath).string();
}

std::string getModelPath(std::string modelLocalPath) {
  return getResPath().append("models").append(modelLocalPath).string();
}

#endif // MY_RESOURCES_H