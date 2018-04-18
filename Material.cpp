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
    m_albeidoTexture = SOIL_load_OGL_texture(diffusePath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    if (m_albeidoTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
  }

  if (!material.roughness_texname.empty())
  {
    std::string roughnessPath = "./Resources/" + material.roughness_texname;
    m_roughnessTexture = SOIL_load_OGL_texture(roughnessPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    if (m_roughnessTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
  }

  if (!material.metallic_texname.empty())
  {
    std::string metallicPath = "./Resources/" + material.metallic_texname;
    m_metallicTexture = SOIL_load_OGL_texture(metallicPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    if (m_metallicTexture == 0)
    {
      std::cerr << "Failed to load texture from file | ";
      std::cerr << SOIL_last_result() << std::endl;
    }
  }
}

void Material::Bind()
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_albeidoTexture);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_metallicTexture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_roughnessTexture);
}