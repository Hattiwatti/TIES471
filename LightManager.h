#pragma once
#include "Light.h"
#include <memory>
#include <vector>

class LightManager
{
public:
  LightManager();
  ~LightManager();

  void CreateLights();
  void DestroyLights();

  void Update(double);
  std::vector<std::unique_ptr<Light>> const& GetLights() { return m_lights; }

private:
  std::vector<std::unique_ptr<Light>> m_lights;

public:
  LightManager(LightManager const&) = delete;
  void operator=(LightManager const) = delete;
};