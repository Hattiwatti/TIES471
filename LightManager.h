#pragma once

class LightManager
{
public:
  LightManager();
  ~LightManager();

  void Update(float);

private:

public:
  LightManager(LightManager const&) = delete;
  void operator=(LightManager const) = delete;
};