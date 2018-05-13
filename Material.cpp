#include "Material.h"
#include <iostream>
#include <SOIL/SOIL.h>
#pragma comment(lib, "SOIL.lib")

Material::Material()
{

}

Material::~Material()
{

}

Material::Material(tinyobj::material_t const& material)
{
  if (!material.diffuse_texname.empty())
  {
    std::string diffusePath = "./Resources/" + material.diffuse_texname;
    m_albeidoTexture = SOIL_load_OGL_texture(diffusePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
    if (m_albeidoTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, m_albeidoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if (!material.bump_texname.empty())
  {
    std::string bumpPath = "./Resources/" + material.bump_texname;
    m_normalTexture = SOIL_load_OGL_texture(bumpPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
    if (m_normalTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, m_normalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if (!material.roughness_texname.empty())
  {
    std::string roughnessPath = "./Resources/" + material.roughness_texname;
    m_roughnessTexture = SOIL_load_OGL_texture(roughnessPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (m_roughnessTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, m_roughnessTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }

  if (!material.metallic_texname.empty())
  {
    std::string metallicPath = "./Resources/" + material.metallic_texname;
    m_metallicIoRTexture = SOIL_load_OGL_texture(metallicPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB);
    if (m_metallicIoRTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, m_metallicIoRTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
}

Material::Material(glm::vec4 const& color, float metallic, float roughness, float IOR)
{
  m_properties.color = color;
  m_properties.metallic = metallic;
  m_properties.roughness = roughness;
  m_properties.IoR = IOR;
  m_properties.useTextures = false;
}

void Material::Bind()
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_albeidoTexture);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_normalTexture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_metallicIoRTexture);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_roughnessTexture);
}