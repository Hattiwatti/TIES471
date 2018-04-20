#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

class ShaderManager
{
public:
  ShaderManager();
  ~ShaderManager();

  void AddShader(std::string const& name, const char* sVertexFile, const char* sFragmentFile);
  void UseShader(std::string const& name);
  void SetUniform1i(const char* uniformName, int value);

private:
  std::unordered_map<std::string, GLuint> m_shaderPrograms;

  std::string m_activeName;
  GLuint m_activeShader;

public:
  ShaderManager(ShaderManager const&) = delete;
  void operator=(ShaderManager const&) = delete;
};