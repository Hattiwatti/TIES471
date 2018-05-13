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

Material::Material(glm::vec3 const& color, float metallic, float roughness, float IOR)
{
  glGenTextures(1, &m_albeidoTexture);
  glBindTexture(GL_TEXTURE_2D, m_albeidoTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, &color);

  glm::vec3 metallicIoR = glm::vec3(metallic, IOR, 0);
  glGenTextures(1, &m_metallicIoRTexture);
  glBindTexture(GL_TEXTURE_2D, m_metallicIoRTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, &metallicIoR);

  glGenTextures(1, &m_roughnessTexture);
  glBindTexture(GL_TEXTURE_2D, m_roughnessTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_R, GL_FLOAT, &roughness);

  glm::vec3 normal = glm::vec3(0, 0, 1);
  glGenTextures(1, &m_normalTexture);
  glBindTexture(GL_TEXTURE_2D, m_normalTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGB, GL_FLOAT, &normal);
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