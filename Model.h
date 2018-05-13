#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
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
        std::shared_ptr<Material> pMaterial);

  void Draw();

  void SetTransform(glm::mat4 const& trans) { m_transform = trans; }
  void SetMaterial(std::shared_ptr<Material> pMat) { m_pMaterial = pMat; }

  Material* GetMaterial() { return m_pMaterial.get(); }
  glm::mat4 const& GetTransform() { return m_transform; }

private:
  GLuint m_indexBuffer;
  GLuint m_normalBuffer;
  GLuint m_vertexBuffer;
  GLuint m_texCoordBuffer;

  int m_indexCount;
  std::shared_ptr<Material> m_pMaterial;

  glm::mat4 m_transform;
};

