#include "Camera.h"
#include <glm/ext.hpp>
#include <Windows.h>

Camera::Camera() :
  m_position(0, 3, 10),
  m_rotation(glm::vec3(0, glm::radians(0.f), 0)),
  m_viewMatrix(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))),
  m_pitch(0),
  m_yaw(3.14f)
{

}

Camera::~Camera()
{

}

void Camera::Update(double dt)
{
  if (GetAsyncKeyState(VK_LEFT) & 0x8000)
  {
    glm::quat qYaw = glm::quat(glm::vec3(0, 0.5*dt, 0));
    m_rotation = m_rotation * qYaw;
    m_yaw += 0.5*dt;
  }
  if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
  {
    glm::quat qYaw = glm::quat(glm::vec3(0, -0.5*dt, 0));
    m_rotation = m_rotation * qYaw;
    m_yaw -= 0.5*dt;
  }

  if (GetAsyncKeyState(VK_UP) & 0x8000)
  {
    glm::quat qPitch = glm::quat(glm::vec3(0.5*dt,0, 0));
    m_rotation = qPitch * m_rotation;
    m_pitch += 0.5*dt;
  }
  if (GetAsyncKeyState(VK_DOWN) & 0x8000)
  {
    glm::quat qPitch = glm::quat(glm::vec3(-0.5*dt,0, 0));
    m_rotation = qPitch * m_rotation;
    m_pitch -= 0.5*dt;
  }

  m_rotation = glm::quat(glm::vec3(m_pitch, m_yaw,0));
  glm::mat4 rotationMatrix = glm::mat4(m_rotation);

  if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    m_position += glm::vec3(rotationMatrix[1]) * (float)dt;
  if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
    m_position += glm::vec3(-rotationMatrix[1]) * (float)dt;

  if (GetAsyncKeyState('W') & 0x8000)
    m_position += glm::vec3(rotationMatrix[2]) * (float)dt;
  if (GetAsyncKeyState('S') & 0x8000)
    m_position += glm::vec3(-rotationMatrix[2]) * (float)dt;

  if (GetAsyncKeyState('A') & 0x8000)
    m_position += glm::vec3(rotationMatrix[0]) * (float)dt;
  if (GetAsyncKeyState('D') & 0x8000)
    m_position += glm::vec3(-rotationMatrix[0]) * (float)dt;


  m_viewMatrix = glm::lookAt(m_position, m_position + glm::vec3(rotationMatrix[2]), glm::vec3(0, 1, 0));
}

