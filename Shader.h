#pragma once
#include <GL/glew.h>

//
// TODO: Remove class and just use program ID
//

class Shader
{
public:
  Shader();
  Shader(const char* vertexShader, const char* fragmentShader);
  ~Shader();

  void Bind();
  GLuint GetID() {
    return m_programID;
  }

private:
  GLuint m_programID;

public:
  Shader(Shader const&) = delete;
  void operator=(Shader const&) = delete;
};