#include "Camera.h"
#include <glm/ext.hpp>
#include <Windows.h>

Camera::Camera() :
  m_position(0, 0, 2),
  m_qRotation(0, 0, 0, 1),
  m_viewMatrix(glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)))
{

}

Camera::~Camera()
{

}

void Camera::Update(double dt)
{


}

