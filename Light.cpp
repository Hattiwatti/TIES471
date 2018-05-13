#include "Light.h"

// +- constraints for random light positions
static const float maxRandX = 20;
static const float maxRandY = 10;
static const float maxRandZ = 20;

static const float lightSpeed = 5.0f;

Light::Light() :
  m_position(0, 0, 0)
{

}

Light::Light(glm::vec3 const& color, float radius) :
  m_position(0, 0, 0),
  m_prevPosition(0,0,0),
  m_interpolator(0)
{
  m_color = color;
  m_radius = radius;

  m_nextPosition.x = (float)(rand()) / ((float)RAND_MAX / (2 * maxRandX)) - maxRandX;
  m_nextPosition.y = (float)(rand()) / ((float)RAND_MAX / (maxRandY));
  m_nextPosition.z = (float)(rand()) / ((float)RAND_MAX / (2 * maxRandZ)) - maxRandZ;
}

Light::~Light()
{

}

void Light::UpdatePosition(double dt)
{
  m_interpolator += dt / lightSpeed;
  m_position = m_prevPosition + (m_nextPosition - m_prevPosition) * m_interpolator;

  if (m_interpolator >= 1.0f)
  {
    m_prevPosition = m_position;
    m_nextPosition.x = (float)(rand()) / ((float)RAND_MAX / (2 * maxRandX)) - maxRandX;
    m_nextPosition.y = (float)(rand()) / ((float)RAND_MAX / (maxRandY));
    m_nextPosition.z = (float)(rand()) / ((float)RAND_MAX / (2 * maxRandZ)) - maxRandZ;
    m_interpolator = 0;
  }
}

