#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
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

  void UpdateViewMatrix(glm::mat4 const& viewMatrix) { m_viewMatrix = viewMatrix; }
  void UpdateViewPos(glm::vec3 const& viewPos) { m_viewPos = viewPos; }

private:
  void CreateBuffers(int width, int height);
  bool CreateShaders();

private:
  GBuffer m_gbuffer;

  GLuint m_screenVbo;
  Shader* m_screenShader;

  glm::mat4 m_viewMatrix;
  glm::mat4 m_projMatrix;
  glm::vec3 m_viewPos;

public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};