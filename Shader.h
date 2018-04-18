#pragma once

class Shader
{
public:
  Shader();
  ~Shader();

  bool CompileProgram(const char* vertexShader, const char* fragmentShader);
  void Bind();

private:
  int m_programID;


public:
  Shader(Shader const&) = delete;
  void operator=(Shader const&) = delete;
};