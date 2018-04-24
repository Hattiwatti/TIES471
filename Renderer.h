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

// Use a struct for now
struct DirectionalLight
{
  glm::mat4 viewMatrix;
  glm::mat4 projMatrix;
  glm::vec3 color;
  GLuint shadowMap;
};

class Renderer
{
public:
  Renderer();
  ~Renderer();

  void Initialize(glm::vec2 const& initialSize);

  void NewFrame();
  void GeometryPass();
  void LightingPass(int method);
  void Present();

  void UpdateMatrices(glm::mat4 const& cameraTransform, float fieldOfView);
  void DrawSkybox();

  void RecompileShaders() { m_shaderManager.Recompile(); }

private:
  void CreateBuffers(int width, int height);
  void CreateShaders();

  void UpdateShadowMap();

private:
  ShaderManager m_shaderManager;

  GBuffer m_gbuffer;
  GLuint m_screenVbo;

  GLuint m_uniformBuffer;
  UniformBlock m_uniformBlock;

  glm::mat4 m_viewMatrix;
  glm::mat4 m_projMatrix;
  glm::vec3 m_viewPos;
  float m_fieldOfView;
  glm::vec2 m_WindowSize;

  GLuint skybox;
  GLuint skyboxVBO;
  GLuint skyboxIBO;
  GLuint skyboxIrradiance;

  GLuint DepthFBO;
  DirectionalLight MyLonelyDirectionalLight;
  bool IWouldLikeToUpdateMyShadowMapsPlease;

public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};