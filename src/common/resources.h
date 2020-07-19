
#include "filesystem.h"

std::string getTexturePath(std::string texLocalPath) {
  return getResPath().append("textures").append(texLocalPath).string();
}

std::string getShaderPath(std::string shaderLocalPath) {
  return getExecPath().append(shaderLocalPath).string();
}