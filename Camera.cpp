#include "Camera.h"
#include <glm/ext.hpp>
#include <Windows.h>

static const float g_cameraMouseSensitivity = 0.1;
static const float g_cameraRotationSpeed = glm::radians(45.f);
static const float g_cameraMovementSpeed = 5.f;

Camera::Camera() :
  m_position(0, 3, 10),
  m_transform(glm::vec4(1, 0, 0, 0), 
              glm::vec4(0, 1, 0, 0), 
              glm::vec4(0, 0, 1, 0), 
              glm::vec4(0, 3,10, 0)),
  m_pitch(0),
  m_yaw(3.14f),
  m_fieldOfView(glm::radians(60.f))
{

}

Camera::~Camera()
{

}

void Camera::Update(double dt)
{
  if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    m_yaw += g_cameraRotationSpeed * dt;

  if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    m_yaw -= g_cameraRotationSpeed * dt;

  if (GetAsyncKeyState(VK_UP) & 0x8000)
    m_pitch -= g_cameraRotationSpeed *dt;

  if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    m_pitch += g_cameraRotationSpeed *dt;

  glm::quat qRotation = glm::quat(glm::vec3(m_pitch, m_yaw,0));
  glm::mat4 rotationMatrix = glm::mat4(qRotation);

  if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    m_position += glm::vec3(rotationMatrix[1]) * g_cameraMovementSpeed * (float)dt;
  if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
    m_position += glm::vec3(-rotationMatrix[1])* g_cameraMovementSpeed * (float)dt;

  if (GetAsyncKeyState('W') & 0x8000)
    m_position += glm::vec3(rotationMatrix[2])* g_cameraMovementSpeed * (float)dt;
  if (GetAsyncKeyState('S') & 0x8000)
    m_position += glm::vec3(-rotationMatrix[2])* g_cameraMovementSpeed * (float)dt;

  if (GetAsyncKeyState('A') & 0x8000)
    m_position += glm::vec3(rotationMatrix[0])* g_cameraMovementSpeed * (float)dt;
  if (GetAsyncKeyState('D') & 0x8000)
    m_position += glm::vec3(-rotationMatrix[0])* g_cameraMovementSpeed * (float)dt;

  rotationMatrix[3] = glm::vec4(m_position,1);
  m_transform = rotationMatrix;
}

void Camera::HandleMouseRotation(double dt, double dX, double dY)
{
  m_pitch += g_cameraRotationSpeed * dt * dY * g_cameraMouseSensitivity;
  m_yaw -= g_cameraRotationSpeed * dt * dX * g_cameraMouseSensitivity;
}