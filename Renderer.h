#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "ShaderManager.h"

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
};

struct UniformBlock
{
  glm::mat4 modelViewProj;
  glm::vec3 cameraPosition;
};


class Renderer
{
public:
  Renderer();
  ~Renderer();

  void Initialize(int width, int height);

  void NewFrame();
  void GeometryPass();
  void LightingPass(int method);
  void Present();

  void UpdateMatrices(glm::mat4 const& cameraTransform);
  void DrawSkybox();

private:
  void CreateBuffers(int width, int height);
  void CreateShaders();

private:
  ShaderManager m_shaderManager;

  GBuffer m_gbuffer;
  GLuint m_screenVbo;

  GLuint m_uniformBuffer;
  UniformBlock m_uniformBlock;
  glm::mat4 m_viewMatrix;
  glm::mat4 m_projMatrix;
  glm::vec3 m_viewPos;

  GLuint skybox;
  GLuint skyboxVBO;
  GLuint skyboxIBO;

public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};