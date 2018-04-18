#pragma once
#include <GL/glew.h>

class Shader
{
public:
  Shader();
  Shader(const char* vertexShader, const char* fragmentShader);
  ~Shader();

  void Bind();

private:
  GLuint m_programID;

public:
  Shader(Shader const&) = delete;
  void operator=(Shader const&) = delete;
};