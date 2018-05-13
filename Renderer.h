#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "ShaderManager.h"
#include "Model.h"

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

struct DebugUniformBlock
{
  float AlbeidoMultiplier{ 1.0f };
  float MetallicMultiplier{ 1.0f };
  float RoughnessMultiplier{ 1.0f };
  float Pad001;
};

struct UniformBlock
{
  glm::mat4 viewProj;
  glm::vec3 cameraPosition;
  DebugUniformBlock debugBlock;
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
  void GeometryPass(std::vector<Model*> const&);
  void LightingPass(int brdf, int debug);
  void Present();

  void UpdateMatrices(glm::mat4 const& cameraTransform, float fieldOfView);
  void DrawSkybox();

  void RecompileShaders() { m_shaderManager.Recompile(); }

  DebugUniformBlock& GetDebugStruct() { return m_uniformBlock.debugBlock; }

private:
  void CreateBuffers(int width, int height);
  void CreateShaders();

  void UpdateShadowMap();

  void DrawIrradiance(int debugMethod);
  void DrawLights(int brdfMethod);

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
  DirectionalLight m_DirectionalLight;
  bool m_UpdateShadowMaps;

public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};