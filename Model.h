#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "Material.h"

struct VertexData
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec2 texCoord;

  VertexData(glm::vec3 pos, glm::vec3 norm, glm::vec3 t, glm::vec2 tex) :
    position(pos),
    normal(norm),
    tangent(t),
    texCoord(tex)
  {

  }
};

class Model
{
public:
  Model();
  ~Model();

  Model(std::vector<VertexData> const& vertices,
        std::vector<GLuint> const& indices,
        Material* pMaterial);

  void Draw();

private:
  GLuint m_indexBuffer;
  GLuint m_normalBuffer;
  GLuint m_vertexBuffer;
  GLuint m_texCoordBuffer;

  int m_indexCount;
  Material* m_pMaterial;

public:
  Model(Model const&) = delete;
  void operator=(Model const&) = delete;
};

