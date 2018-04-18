#pragma once
#include <GL/glew.h>
#include "Shader.h"

enum BRDFMethod
{
  BlinnPhong,
  CookTorrance
};

struct GBuffer
{
  GLuint fbo;
  GLuint depth;
  GLuint position;
  GLuint normals;
  GLuint albedoMetallic;
  GLuint roughness;
  Shader* shader;
};

class Renderer
{
public:
  Renderer();
  ~Renderer();

  void Initialize(int width, int height);

  void NewFrame();
  void SetupGeometryPass();
  void SetupLightingPass(int method);
  void Present();

private:
  void CreateBuffers(int width, int height);
  bool CreateShaders();

private:
  GBuffer m_gbuffer;

  GLuint m_screenVbo;
  GLuint m_screenVao;
  Shader* m_screenShader;

public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};