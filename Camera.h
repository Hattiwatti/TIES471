#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
  Camera();
  ~Camera();

  void Update(double dt);
  void HandleMouseRotation(double dt, double dX, double dY);

  const glm::mat4 GetTransform() { return m_transform; }
  const float GetFov() { return m_fieldOfView; }

private:
  glm::vec3 m_position;

  float m_pitch;
  float m_yaw;
  float m_roll;
  float m_fieldOfView;

  glm::mat4 m_transform;

public:
  Camera(Camera const&) = delete;
  void operator=(Camera const&) = delete;
};