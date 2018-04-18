#include "Model.h"

Model::Model() :
  m_indexBuffer(0),
  m_vertexBuffer(0),
  m_normalBuffer(0),
  m_material(-1),
  m_indexCount(0)
{

}

Model::~Model()
{

}

Model::Model( std::vector<glm::vec3> const& vertices, 
              std::vector<glm::vec3> const& normals, 
              std::vector<glm::vec3> const& texCoords, 
              std::vector<GLuint> const& indices, int materialIndex)
{
  m_material = materialIndex;
  m_indexCount = indices.size();

  glGenBuffers(1, &m_indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_normalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
  glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float) * 3, &normals[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_texCoordBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
  glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float) * 2, &texCoords[0], GL_STATIC_DRAW);
}

void Model::Draw()
{
  glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
  glTexCoordPointer(2, GL_FLOAT, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
  glNormalPointer(GL_FLOAT, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
  glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL);
}