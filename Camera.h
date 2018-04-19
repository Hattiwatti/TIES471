#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
  Camera();
  ~Camera();

  void Update(double dt);
  const glm::mat4 GetViewMatrix() { return m_viewMatrix; }
  const glm::vec3 GetPosition() { return m_position; }

private:
  glm::vec3 m_position;
  glm::quat m_rotation;

  float m_pitch;
  float m_yaw;
  float m_roll;

  glm::mat4 m_viewMatrix;

public:
  Camera(Camera const&) = delete;
  void operator=(Camera const&) = delete;
};