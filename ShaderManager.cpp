#include "ShaderManager.h"
#include <fstream>
#include <iostream>

static GLint compileShader(const char* shaderPath, GLenum type)
{
  std::ifstream fs(shaderPath, std::ios::binary | std::ios::ate);
  if (!fs || !fs.is_open())
  {
    std::cerr << "Failed to open shader file " << shaderPath << std::endl;
    return 0;
  }

  std::streampos fileSize = fs.tellg();
  fs.seekg(std::ios::beg);
  std::string sShaderCode(static_cast<unsigned int>(fileSize), 0);
  fs.read(&sShaderCode[0], fileSize);

  const GLchar* shaderSource = sShaderCode.c_str();
  const GLint shader = glCreateShader(type);
  if (!shader)
  {
    std::cerr << "Failed to create a shader of type " << type << std::endl;
    abort();
  }

  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
  {
    std::cerr << "Failed to compile shader " << shaderPath << std::endl;

    GLint errorLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);
    GLchar* errorString = new GLchar[errorLength];

    std::vector<GLchar> errorLog(errorLength);
    glGetShaderInfoLog(shader, errorLength, &errorLength, &errorLog[0]);

    std::cerr << (char*)&errorLog[0] << std::endl;

    abort();
  }

  return shader;
}

static GLint compileShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
  const GLint shaderProgram = glCreateProgram();
  if (!shaderProgram)
  {
    std::cerr << "Failed to create shader program" << std::endl;
    abort();
  }

  glAttachShader(shaderProgram, compileShader(vertexShaderPath, GL_VERTEX_SHADER));
  glAttachShader(shaderProgram, compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER));
  glLinkProgram(shaderProgram);

  GLint linked;
  glGetShaderiv(shaderProgram, GL_LINK_STATUS, &linked);
  if (!linked)
  {
    std::cerr << "Failed to link shader program with shaders " << vertexShaderPath << " and " << fragmentShaderPath << std::endl;
    abort();
  }

  return shaderProgram;
}

ShaderManager::ShaderManager() :
  m_activeShader(0)
{

}

ShaderManager::~ShaderManager()
{

}

void ShaderManager::AddShader(std::string const& name, const char* sVertexFile, const char* sFragmentFile)
{
  GLuint programID = compileShaderProgram(sVertexFile, sFragmentFile);

  Shader newShader(name, sVertexFile, sFragmentFile);
  newShader.programId = programID;

  m_shaderPrograms.insert({ name, newShader });
}

void ShaderManager::UseShader(std::string const& name)
{
  auto result = m_shaderPrograms.find(name);
  if (result == m_shaderPrograms.end())
  {
    std::cerr << "Shader \"" << name << "\" could not be found" << std::endl;
    abort();
  }

  m_activeShader = &result->second;
  glUseProgram(m_activeShader->programId);
}

void ShaderManager::SetUniform1i(const char* uniformName, int value)
{
  GLuint uniformLocation = GetUniformLocation(uniformName);
  glUniform1i(uniformLocation, value);
}

void ShaderManager::SetUniform1f(const char* uniformName, float value)
{
  GLuint uniformLocation = GetUniformLocation(uniformName);
  glUniform1f(uniformLocation, value);
}

void ShaderManager::SetUniform3f(const char* uniformName, glm::vec3 const& value)
{
  GLuint uniformLocation = GetUniformLocation(uniformName);
  glUniform3fv(uniformLocation, 1, &value[0]);
}

void ShaderManager::SetUniformMatrix(const char* uniformName, glm::mat4 const& value)
{
  GLuint uniformLocation = GetUniformLocation(uniformName);
  glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &value[0][0]);
}

void ShaderManager::Recompile()
{
  // Recompile shaders to allow editing

  glUseProgram(GL_NONE);
  m_activeShader = nullptr;

  for (auto& entry : m_shaderPrograms)
  {
    Shader& shader = entry.second;
    glDeleteProgram(shader.programId);

    shader.programId = compileShaderProgram(shader.vertexFile.c_str(), shader.fragmentFile.c_str());
  }
}

GLuint ShaderManager::GetProgramID(std::string const& name)
{
  auto result = m_shaderPrograms.find(name);
  if (result == m_shaderPrograms.end())
  {
    std::cerr << "Shader \"" << name << "\" could not be found" << std::endl;
    abort();
  }

  return result->second.programId;
}

GLuint ShaderManager::GetUniformLocation(std::string const& uniformName)
{
  // Lookup uniform locations from a hashed map.
  // If not found, try to add them and raise an error if the uniform doesn't exist

  auto result = m_activeShader->uniformLocations.find(uniformName);
  if (result == m_activeShader->uniformLocations.end())
  {
    GLuint uniformLocation = glGetUniformLocation(m_activeShader->programId, uniformName.c_str());
    if (uniformLocation == -1)
    {
      std::cerr << "Uniform \"" << uniformName << "\" does not exist in " << m_activeShader->name << std::endl;
      abort();
    }

    m_activeShader->uniformLocations.emplace(uniformName, uniformLocation);
    return uniformLocation;
  }

  return result->second;
}
