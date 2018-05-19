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
  void SetUniform1f(const char* uniformName, float value);
  void SetUniform3f(const char* uniformName, glm::vec3 const& value);
  void SetUniformMatrix(const char* uniformName, glm::mat4 const& value);

  void Recompile();
  GLuint GetProgramID(std::string const& name);

private:
  GLuint GetUniformLocation(std::string const& uniformName);

private:
  std::unordered_map<std::string, Shader> m_shaderPrograms;
  Shader* m_activeShader;

public:
  ShaderManager(ShaderManager const&) = delete;
  void operator=(ShaderManager const&) = delete;
};