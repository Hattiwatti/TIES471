#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

class Material
{
public:
  Material();
  Material(tinyobj::material_t const& material);
  Material(glm::vec3 const& color, float metallic, float roughness, float IOR);
  ~Material();

  void Bind();

private:
  GLuint m_albeidoTexture;
  GLuint m_normalTexture;
  GLuint m_metallicIoRTexture;
  GLuint m_roughnessTexture;

public:
  Material(Material const&) = delete;
  void operator=(Material const&) = delete;
};