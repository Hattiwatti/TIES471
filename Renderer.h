#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "ShaderManager.h"
#include "Model.h"
#include "Light.h"

struct GBuffer
{
  GLuint FBO;
  GLuint DepthStencil;
  GLuint Position;
  GLuint Normal;
  GLuint Albedo;
  GLuint Surface;
};

struct DebugUniformBlock
{
  float AlbeidoMultiplier{ 1.0f };
  float MetallicMultiplier{ 1.0f };
  float RoughnessMultiplier{ 1.0f };
  float HardcodedSpecular{ 128.f };
  int debugMode{ 0 };
  int brdfMethod{ 1 };
  float Pad001[3];
};

struct UniformBlock
{
  glm::mat4 viewProj;
  glm::vec3 cameraPosition;
  DebugUniformBlock debugBlock;
};

struct Skybox
{
  GLuint DiffuseTexture;
  GLuint IrradianceTexture;
  GLuint VBO;
  GLuint IBO;

  GLuint PrefilteredTexture[7];
  GLuint BRDFlut;
};

struct View
{
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;
  glm::vec3 EyePosition;
  glm::vec2 WindowSize;
  float FieldOfView;
};

struct RenderOptions
{
  bool DrawLights{ true };
  bool DrawSun{ true };
  bool DrawIndirect{ true };
  bool DrawSkybox{ true };
};

class Renderer
{
public:
  Renderer();
  ~Renderer();

  void Initialize(glm::vec2 const& initialSize);

  void NewFrame();
  void DrawGeometry(std::vector<Model*> const&, std::vector<std::unique_ptr<Light>> const&);

  void UpdateMatrices(glm::mat4 const& cameraTransform, float fieldOfView);
  void DrawSkybox();

  void RecompileShaders() { m_pShaderManager->Recompile(); }

  DebugUniformBlock& GetDebugStruct() { return m_UniformBlock.debugBlock; }
  RenderOptions& GetOptions() { return m_Options; }

private:
  void CreateBuffers(int width, int height);
  void CreateShaders();

  void UpdateShadowMap();

  void DrawIrradiance();
  void DrawLights(std::vector<std::unique_ptr<Light>> const&);

  void GeometryPass(std::vector<Model*> const&);
  void LightingPass(std::vector<std::unique_ptr<Light>> const& lights);

private:
  std::unique_ptr<ShaderManager> m_pShaderManager;

  GBuffer m_GBuffer;
  GLuint m_ScreenVBO;

  GLuint m_UniformBuffer;
  UniformBlock m_UniformBlock;

  Skybox m_Skybox;
  View m_View;

  RenderOptions m_Options;

public:
  Renderer(Renderer const&) = delete;
  void operator=(Renderer const&) = delete;
};