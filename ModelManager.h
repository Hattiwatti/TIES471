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
  void CreateSphereGrid();

  std::vector<Model*> const& GetModels() { return m_models; }

private:


private:
  std::vector<Model*> m_models;
  std::vector<std::shared_ptr<Material>> m_materials;

public:
  ModelManager(ModelManager const&) = delete;
  void operator=(ModelManager const&) = delete;
};