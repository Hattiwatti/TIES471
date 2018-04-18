#pragma once
#include "Material.h"
#include "Model.h"
#include <vector>

class ModelManager
{
public:
  ModelManager();
  ~ModelManager();

  void LoadObj(const char* filename);
  void Draw();

private:
  std::vector<Model*> m_models;
  std::vector<Material*> m_materials;

public:
  ModelManager(ModelManager const&) = delete;
  void operator=(ModelManager const&) = delete;
};