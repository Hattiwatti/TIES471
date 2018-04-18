
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

  std::cout << "Creating models..." << std::endl;

  for (tinyobj::shape_t& shape : shapes)
  {
    std::vector<GLuint> shapeIndices;
    std::vector<VertexData> vertices;
    //std::cout << shape.name << " | ";

    size_t index_offset = 0;
    size_t indexCount = 0;
    int materialIndex = shape.mesh.material_ids[0];
    for (unsigned char faceIndex : shape.mesh.num_face_vertices)
    {
      if (shape.mesh.material_ids[indexCount] != materialIndex)
      {
        //std::cout << shapeIndices.size() << " indices" << std::endl;
        m_models.emplace_back(new Model(vertices, shapeIndices, materialIndex));
        shapeIndices.clear();
        vertices.clear();
        materialIndex = shape.mesh.material_ids[indexCount];
      }

      for (size_t v = 0; v < faceIndex; v++)
      {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

        glm::vec3 pos(attrib.vertices[3 * idx.vertex_index + 0],
          attrib.vertices[3 * idx.vertex_index + 1],
          attrib.vertices[3 * idx.vertex_index + 2]);

        glm::vec3 normal(attrib.normals[3 * idx.normal_index + 0],
          attrib.normals[3 * idx.normal_index + 1],
          attrib.normals[3 * idx.normal_index + 2]);

        glm::vec2 tex(attrib.texcoords[2 * idx.texcoord_index + 0],
          attrib.texcoords[2 * idx.texcoord_index + 1]);

        vertices.emplace_back(pos, normal, tex);
        shapeIndices.emplace_back(vertices.size() - 1);
      }

      index_offset += faceIndex;
      indexCount++;
    }

    //std::cout << shapeIndices.size() << " indices" << std::endl;
    m_models.push_back(new Model(vertices, shapeIndices, materialIndex));
  }

  std::cout << "Created " << m_models.size() << " models from " << sFilename << std::endl;

  std::cout << "Parsing materials" << std::endl;
  for (auto& material : materials)
    m_materials.push_back(new Material(material));

  std::cout << "Created " << m_materials.size() << " materials";
}

void ModelManager::Draw()
{

  for (auto& model : m_models)
  {
    int materialIndex = model->GetMaterial();
    if (materialIndex > -1)
      m_materials[materialIndex]->Bind();

    model->Draw();
  }

}
