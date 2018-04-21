#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>

struct ShaderInfo
{
  std::string const name;
  std::string const vertexFile;
  std::string const fragmentFile;

  ShaderInfo(std::string _name, std::string _vertex, std::string _fragment) :
    name(_name),
    vertexFile(_vertex),
    fragmentFile(_fragment)
  {
  }
};

class ShaderManager
{
public:
  ShaderManager();
  ~ShaderManager();

  void AddShader(std::string const& name, const char* sVertexFile, const char* sFragmentFile);
  void UseShader(std::string const& name);

  void SetUniform1i(const char* uniformName, int value);
  void SetUniformMatrix(const char* uniformName, glm::mat4 const& value);

  void Recompile();

private:
  GLuint GetUniformLocation(const char* uniformName);

private:
  std::unordered_map<std::string, GLuint> m_shaderPrograms;
  std::vector<ShaderInfo> m_shaderFiles;

  std::string m_activeName;
  GLuint m_activeShader;

public:
  ShaderManager(ShaderManager const&) = delete;
  void operator=(ShaderManager const&) = delete;
};