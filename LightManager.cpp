#include "LightManager.h"
#include <time.h>

static const int g_lightCount = 10;

LightManager::LightManager()
{

}

LightManager::~LightManager()
{

}

void LightManager::CreateLights()
{
  // Create a huge amount of lights so performance impact of the functions
  // can be seen more clearly

  srand(time(NULL));

  for (int i = 0; i < g_lightCount; ++i)
  {
    float red = 0.0f + (rand() % 21) / 100.f;
    float green = 0.0f + (rand() % 21) / 100.f;
    float blue = 0.0f + (rand() % 21) / 100.f;

    glm::vec3 randomColor(red, green, blue);
    m_lights.emplace_back(std::make_unique<Light>(randomColor, 10.f));
  }
}

void LightManager::DestroyLights()
{
  m_lights.clear();
}

void LightManager::Update(double dt)
{
  for (auto& light : m_lights)
    light->UpdatePosition(dt);
}