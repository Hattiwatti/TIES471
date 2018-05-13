#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
  Light();
  ~Light();

  void UpdatePosition(double);

private:
  glm::vec3 m_color;
  glm::vec3 m_position;
  float m_radius;

  glm::mat4 m_projection;
  glm::mat4 m_view;
  GLuint m_shadowMap;

public:
  Light(Light const&) = delete;
  void operator=(Light const&) = delete;
};