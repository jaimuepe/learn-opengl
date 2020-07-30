
#ifndef MY_RESOURCES_H
#define MY_RESOURCES_H

#include "filesystem.h"

std::string getTexturePath(const std::string &texLocalPath) {
  return getResPath().append("textures").append(texLocalPath).string();
}

std::string getSkyboxPath(const std::string &name,
                          const std::string &pieceFilename) {
  return getResPath()
      .append("cubemaps")
      .append(name)
      .append(pieceFilename)
      .string();
}

std::string getShaderPath(const std::string &shaderLocalPath) {
  return getExecPath().append(shaderLocalPath).string();
}

std::string getModelPath(const std::string &modelLocalPath) {
  return getResPath().append("models").append(modelLocalPath).string();
}

#endif // MY_RESOURCES_H