#pragma once
#include <map>
#include <string>

#include "shader.h"
#include "texture.h"

class ResourceManager {
 public:
  static std::map<std::string, Shader> Shaders;
  static std::map<std::string, Texture2D> Textures;
  static Shader loadShader(const char *vShaderFile, const char *fShaderFile,
                           const char *gShaderFile, std::string name);
  static Shader getShader(std::string name);
  static Texture2D loadTexture(const char *file, bool alpha, std::string name);
  static Texture2D getTexture(std::string name);
  static void clear();

 private:
  ResourceManager() {}
  static Shader loadShaderFromFile(const char *vShaderFile,
                                   const char *fShaderFile,
                                   const char *gShaderFile = nullptr);
  static Texture2D loadTextureFromFile(const char *file, bool alpha);
};