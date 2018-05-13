#include "Model.h"

Model::Model() :
  m_indexBuffer(0),
  m_vertexBuffer(0),
  m_normalBuffer(0),
  m_indexCount(0),
  m_transform(glm::mat4(1.0f))
{

}

Model::~Model()
{

}

Model::Model( std::vector<VertexData> const& vertices, 
              std::vector<GLuint> const& indices, std::shared_ptr<Material> pMaterial)
{
  m_transform = glm::mat4(1.0f);
  m_pMaterial = pMaterial;
  m_indexCount = indices.size();

  glGenBuffers(1, &m_indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &m_vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);
}

void Model::Draw()
{
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0xC);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0x18);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0x24);

  glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, NULL);
}