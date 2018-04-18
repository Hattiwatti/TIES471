#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Model
{
public:
  Model();
  ~Model();

  Model(std::vector<glm::vec3> const& vertices,
        std::vector<glm::vec3> const& normals,
        std::vector<glm::vec3> const& texCoords,
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
