#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>

struct MaterialProperties
{
  bool useTextures{ true };
  glm::vec3 color{ 0,0,0 };
  float metallic{ 0 };
  float roughness{ 0 };
  float IoR{ 0 };
};

class Material
{
public:
  Material();
  Material(tinyobj::material_t const& material);
  Material(glm::vec4 const& color, float metallic, float roughness, float IOR);
  ~Material();

  void Bind();
  MaterialProperties const& GetProperties() { return m_properties; }

private:
  GLuint m_albeidoTexture;
  GLuint m_normalTexture;
  GLuint m_metallicIoRTexture;
  GLuint m_roughnessTexture;

  MaterialProperties m_properties;

public:
  Material(Material const&) = delete;
  void operator=(Material const&) = delete;
};