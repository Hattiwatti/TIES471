#pragma once

class ModelManager
{
public:
  ModelManager();
  ~ModelManager();

  bool LoadObj(const char* filename);
  void Draw();

private:

private:

public:
  ModelManager(ModelManager const&) = delete;
  void operator=(ModelManager const&) = delete;
};