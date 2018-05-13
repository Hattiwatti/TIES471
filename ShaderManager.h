#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <vector>

struct Shader
{
  std::string const name;
  std::string const vertexFile;
  std::string const fragmentFile;

  GLuint programId;
  std::unordered_map<std::string, GLuint> uniformLocations;

  Shader(std::string _name, std::string vsFile, std::string fsFile) :
    name(_name),
    vertexFile(vsFile),
    fragmentFile(fsFile)
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
  GLuint GetUniformLocation(std::string const& uniformName);

private:
  std::unordered_map<std::string, Shader> m_shaderPrograms;
  Shader* m_activeShader;

public:
  ShaderManager(ShaderManager const&) = delete;
  void operator=(ShaderManager const&) = delete;
};