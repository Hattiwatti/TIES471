#include "Shader.h"
#include <fstream>
#include <iostream>
#include <vector>

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

Shader::Shader() :
  m_programID(0)
{

}

Shader::~Shader()
{

}

Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
  m_programID = compileShaderProgram(vertexShader, fragmentShader);
}

void Shader::Bind()
{
  glUseProgram(m_programID);
}