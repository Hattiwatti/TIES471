#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
  Camera();
  ~Camera();

  void Update(double dt);
  const glm::mat4 GetTransform() { return m_transform; }

private:
  glm::vec3 m_position;

  float m_pitch;
  float m_yaw;
  float m_roll;

  glm::mat4 m_transform;

public:
  Camera(Camera const&) = delete;
  void operator=(Camera const&) = delete;
};