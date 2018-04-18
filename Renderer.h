#pragma once

enum BRDFMethod
{
  Lambertian,
  BlinnPhong,
  CookTorrance
};

class Renderer
{
public:
  Renderer();
  ~Renderer();

  bool Initialize();

  void NewFrame();
  void SetupGeometryPass();
  void SetupLightingPass(int method);
  void Present();

  void UpdateViewMatrix();

private:
  bool CreateBuffers();
  bool CreateShaders();


private:


public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};