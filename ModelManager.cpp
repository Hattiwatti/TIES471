
#include "ModelManager.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


ModelManager::ModelManager()
{

}

ModelManager::~ModelManager()
{

}

void ModelManager::LoadObj(const char* sFilename)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  std::cout << "Loading obj..." << std::endl;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, sFilename, "./Resources/", true))
  {
    std::cerr << "Failed to load models from " << sFilename << std::endl;
    if (!err.empty())
      std::cerr << err << std::endl;

    abort();
  }

  std::cout << "Parsing materials" << std::endl;
  for (auto& material : materials)
    m_materials.push_back( std::make_shared<Material>(material) );

  std::cout << "Creating models..." << std::endl;

  for (tinyobj::shape_t& shape : shapes)
  {
    std::vector<GLuint> shapeIndices;
    std::vector<VertexData> vertices;
    //std::cout << shape.name << " | ";

    size_t index_offset = 0;
    size_t indexCount = 0;

    std::shared_ptr<Material> pMaterial = nullptr;
    int materialIndex = shape.mesh.material_ids[0];
    if (materialIndex >= 0)
      pMaterial = m_materials[materialIndex];

    for (unsigned char faceIndex : shape.mesh.num_face_vertices)
    {
      if (shape.mesh.material_ids[indexCount] != materialIndex)
      {
        //std::cout << shapeIndices.size() << " indices" << std::endl;
        m_models.push_back(new Model(vertices, shapeIndices, pMaterial));
        shapeIndices.clear();
        vertices.clear();
        materialIndex = shape.mesh.material_ids[indexCount];

        pMaterial.reset();
        if (materialIndex >= 0)
          pMaterial = m_materials[materialIndex];
      }

      VertexData v[3];
      for (int i = 0; i < 3; ++i)
      {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + i];

        v[i].position = glm::vec3(attrib.vertices[3 * idx.vertex_index + 0],
          attrib.vertices[3 * idx.vertex_index + 1],
          attrib.vertices[3 * idx.vertex_index + 2]);

        v[i].normal = glm::vec3(attrib.normals[3 * idx.normal_index + 0],
          attrib.normals[3 * idx.normal_index + 1],
          attrib.normals[3 * idx.normal_index + 2]);

        v[i].texCoord = glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0],
          attrib.texcoords[2 * idx.texcoord_index + 1]);
      }

      glm::vec3 deltaPos1 = v[1].position - v[0].position;
      glm::vec3 deltaPos2 = v[2].position - v[0].position;
      glm::vec2 deltaUV1 = v[1].texCoord - v[0].texCoord;
      glm::vec2 deltaUV2 = v[2].texCoord - v[0].texCoord;

      float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
      glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);

      for (int i = 0; i < 3; ++i)
        v[i].tangent = tangent;

      vertices.push_back(v[0]);
      shapeIndices.emplace_back(vertices.size() - 1);
      vertices.push_back(v[1]);
      shapeIndices.emplace_back(vertices.size() - 1);
      vertices.push_back(v[2]);
      shapeIndices.emplace_back(vertices.size() - 1);

      index_offset += faceIndex;
      indexCount++;
    }

    //std::cout << shapeIndices.size() << " indices" << std::endl;
    m_models.push_back(new Model(vertices, shapeIndices, pMaterial));
  }

  std::cout << "Created " << m_models.size() << " models from " << sFilename << std::endl;
  std::cout << "Created " << m_materials.size() << " materials" << std::endl;
}

void ModelManager::CreateSphereGrid()
{
  glm::mat4 transform(1.0f);
  transform[3][0] = -6.25f;
  transform[3][1] = 2.f;

  glm::vec4 color(1.0f, 0.2f, 0.2f, 1.0f);
  float metallic = 0.1f;
  float roughness = 0.1f;
  float IoR = 0.5f;

  LoadObj("./Resources/Sphere.obj");
  Model* pSphere = m_models[m_models.size() - 1];
  pSphere->SetMaterial(std::make_shared<Material>(color, metallic, roughness, IoR));
  pSphere->SetTransform(transform);
  transform[3][0] += 2.5f;

  for (int i = 0; i < 5; ++i)
  {
    metallic = (i == 0) ? 0.1f : i / 4.f;

    for (int j = 0; j < 5; ++j)
    {
      if (j == 0 && i == 0) continue;
      roughness = (j == 0) ? 0.1f : j / 4.f;

      Model* newSphere = new Model(*pSphere);
      newSphere->SetMaterial(std::make_shared<Material>(color, metallic, roughness, IoR));
      newSphere->SetTransform(transform);
      m_models.push_back(newSphere);

      transform[3][0] += 2.5f;
    }

    transform[3][0] = -6.25f;
    transform[3][1] += 2.5f;
  }

  metallic = 0;
  roughness = 0.5f;

  transform[3][1] += 1.5f;

  for (int i = 0; i < 5; ++i)
  {
    IoR = i / 4.f;
    Model* newSphere = new Model(*pSphere);
    newSphere->SetMaterial(std::make_shared<Material>(color, metallic, roughness, IoR));
    newSphere->SetTransform(transform);
    m_models.push_back(newSphere);
    transform[3][0] += 2.5f;
  }
}