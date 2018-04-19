#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
  Camera();
  ~Camera();

  void Update(double dt);
  const glm::mat4 GetViewMatrix() { return m_viewMatrix; }

private:
  glm::vec3 m_position;
  glm::vec4 m_qRotation;

  glm::mat4 m_viewMatrix;

public:
  Camera(Camera const&) = delete;
  void operator=(Camera const&) = delete;
};