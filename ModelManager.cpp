
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

      tinyobj::index_t idx0 = shape.mesh.indices[index_offset + 0];
      tinyobj::index_t idx1 = shape.mesh.indices[index_offset + 1];
      tinyobj::index_t idx2 = shape.mesh.indices[index_offset + 2];

      glm::vec3 v0 = glm::vec3(attrib.vertices[3 * idx0.vertex_index + 0],
                                attrib.vertices[3 * idx0.vertex_index + 1],
                                attrib.vertices[3 * idx0.vertex_index + 2]);

      glm::vec3 v1 = glm::vec3(attrib.vertices[3 * idx1.vertex_index + 0],
                                attrib.vertices[3 * idx1.vertex_index + 1],
                                attrib.vertices[3 * idx1.vertex_index + 2]);

      glm::vec3 v2 = glm::vec3(attrib.vertices[3 * idx2.vertex_index + 0],
                                attrib.vertices[3 * idx2.vertex_index + 1],
                                attrib.vertices[3 * idx2.vertex_index + 2]);


      glm::vec3 n0 = glm::vec3(attrib.normals[3 * idx0.normal_index + 0],
        attrib.normals[3 * idx0.vertex_index + 1],
        attrib.normals[3 * idx0.vertex_index + 2]);

      glm::vec3 n1 = glm::vec3(attrib.normals[3 * idx1.normal_index + 0],
        attrib.normals[3 * idx1.vertex_index + 1],
        attrib.normals[3 * idx1.vertex_index + 2]);

      glm::vec3 n2 = glm::vec3(attrib.normals[3 * idx2.normal_index + 0],
        attrib.normals[3 * idx2.vertex_index + 1],
        attrib.normals[3 * idx2.vertex_index + 2]);

      glm::vec2 uv0 = glm::vec2(attrib.texcoords[2 * idx0.texcoord_index + 0],
        attrib.texcoords[2 * idx0.texcoord_index + 1]);

      glm::vec2 uv1 = glm::vec2(attrib.texcoords[2 * idx1.texcoord_index + 0],
        attrib.texcoords[2 * idx1.texcoord_index + 1]);

      glm::vec2 uv2 = glm::vec2(attrib.texcoords[2 * idx2.texcoord_index + 0],
        attrib.texcoords[2 * idx2.texcoord_index + 1]);

      glm::vec3 deltaPos1 = v1 - v0;
      glm::vec3 deltaPos2 = v2 - v0;
      glm::vec2 deltaUV1 = uv1 - uv0;
      glm::vec2 deltaUV2 = uv2 - uv0;

      float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
      glm::vec3 tangent = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);

      vertices.emplace_back(v0, n0, tangent, uv0);
      shapeIndices.emplace_back(vertices.size() - 1);
      vertices.emplace_back(v1, n1, tangent, uv1);
      shapeIndices.emplace_back(vertices.size() - 1);
      vertices.emplace_back(v2, n2, tangent, uv2);
      shapeIndices.emplace_back(vertices.size() - 1);

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
