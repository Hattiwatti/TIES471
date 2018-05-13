#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
  Light();
  Light(glm::vec3 const& color, float radius);
  ~Light();

  void UpdatePosition(double);

  glm::vec3 const GetColor() { return m_color; }
  glm::vec3 const GetPosition() { return m_position; }
  float const GetRadius() { return m_radius; }

private:
  glm::vec3 m_color;
  glm::vec3 m_position;
  float m_radius;

  glm::mat4 m_projection;
  glm::mat4 m_view;
  GLuint m_shadowMap;

  glm::vec3 m_prevPosition;
  glm::vec3 m_nextPosition;
  float m_interpolator;

public:
  Light(Light const&) = delete;
  void operator=(Light const&) = delete;
};