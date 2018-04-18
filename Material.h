#pragma once
#include <GL/glew.h>

class Material
{
public:
  Material();
  ~Material();

  void Bind();

private:


public:
  Material(Material const&) = delete;
  void operator=(Material const&) = delete;
};