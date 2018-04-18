#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct VertexData
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;

  VertexData(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) :
    position(pos),
    normal(norm),
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
        int materialIndex);

  void Draw();
  int GetMaterial() { return m_material; }

private:
  GLuint m_indexBuffer;
  GLuint m_normalBuffer;
  GLuint m_vertexBuffer;
  GLuint m_texCoordBuffer;

  int m_indexCount;
  int m_material;

public:
  Model(Model const&) = delete;
  void operator=(Model const&) = delete;
};
