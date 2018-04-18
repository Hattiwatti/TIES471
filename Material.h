#pragma once
#include <GL/glew.h>
#include <tiny_obj_loader.h>

class Material
{
public:
  Material();
  Material(tinyobj::material_t const& material);
  ~Material();

  void Bind();

private:
  GLuint m_albeidoTexture;
  GLuint m_normalTexture;
  GLuint m_metallicTexture;
  GLuint m_roughnessTexture;

public:
  Material(Material const&) = delete;
  void operator=(Material const&) = delete;
};