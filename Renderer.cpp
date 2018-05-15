#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <SOIL/SOIL.h>

static const glm::vec3 screenVertices[4] =
{
  glm::vec3(-1.f, -1.f, 0),
  glm::vec3(1.f,  -1.f, 0),
  glm::vec3(1.f,  1.f, 0),
  glm::vec3(-1.f, 1.f, 0)
};

static const glm::vec3 skyboxVertices[24] = 
{
  {-1000,-1000, 1000 },
  {-1000, 1000, 1000 },
  { 1000, 1000, 1000 },
  { 1000,-1000, 1000 },

  {-1000,-1000, -1000 },
  {-1000, 1000, -1000 },
  { 1000, 1000, -1000 },
  { 1000,-1000, -1000 },

  {  1000,-1000,-1000 },
  {  1000,-1000, 1000 },
  {  1000, 1000, 1000 },
  {  1000, 1000,-1000 },

  { -1000,-1000, 1000 },
  { -1000,-1000,-1000 },
  { -1000, 1000,-1000 },
  { -1000, 1000, 1000 },

  { 1000, 1000, 1000 },
  {-1000, 1000, 1000 },
  {-1000, 1000,-1000 },
  { 1000, 1000,-1000 },

  { 1000,-1000, 1000 },
  { 1000,-1000,-1000 },
  {-1000,-1000,-1000 },
  {-1000,-1000, 1000 }
};

static GLuint LoadCubemap(const char* filename)
{
  GLuint cubemap = SOIL_load_OGL_single_cubemap(filename, "WNESUD", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);
  if (cubemap == 0)
  {
    std::cerr << "Failed to load cubemap from file " << filename << std::endl;
    std::abort();
  }
  
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return cubemap;
}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Initialize(glm::vec2 const& initialSize)
{
  m_View.WindowSize = initialSize;
  m_View.FieldOfView = glm::radians(50.f);

  m_View.ProjectionMatrix = glm::perspective(m_View.FieldOfView, initialSize.x/initialSize.y, 0.1f, 10000.f);
  CreateBuffers(initialSize.x, initialSize.y);
  CreateShaders();

  m_Skybox.DiffuseTexture = LoadCubemap("./Resources/Textures/miramar/miramar.png");
  m_Skybox.IrradianceTexture = LoadCubemap("./Resources/Textures/miramar/miramar_irradiance.png");

  m_Skybox.BRDFlut = SOIL_load_OGL_texture("./Resources/Textures/ibl_brdf_lut.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);
  glBindTexture(GL_TEXTURE_2D, m_Skybox.BRDFlut);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  m_Skybox.PrefilteredTexture[0] = LoadCubemap("./Resources/Textures/miramar/pmrem_0.png");
  m_Skybox.PrefilteredTexture[1] = LoadCubemap("./Resources/Textures/miramar/pmrem_1.png");
  m_Skybox.PrefilteredTexture[2] = LoadCubemap("./Resources/Textures/miramar/pmrem_2.png");
  m_Skybox.PrefilteredTexture[3] = LoadCubemap("./Resources/Textures/miramar/pmrem_3.png");
  m_Skybox.PrefilteredTexture[4] = LoadCubemap("./Resources/Textures/miramar/pmrem_4.png");
  m_Skybox.PrefilteredTexture[5] = LoadCubemap("./Resources/Textures/miramar/pmrem_5.png");
  m_Skybox.PrefilteredTexture[6] = LoadCubemap("./Resources/Textures/miramar/pmrem_6.png");

  glGenBuffers(1, &m_Skybox.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_Skybox.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::CreateBuffers(int width, int height)
{
  // Create frame buffer and geometry textures for deferred rendering
  // https://learnopengl.com/Advanced-Lighting/Deferred-Shading

  // At the moment the textures are named based on the PBR metallic/roughness
  // workflow, first goal is to get Cook-Torrance working. 

  glGenFramebuffers(1, &m_GBuffer.FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.FBO);

  // DEPTH-STENCIL TEXTURE
  glGenTextures(1, &m_GBuffer.DepthStencil);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.DepthStencil);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_GBuffer.DepthStencil, 0);

  // POSITION GEOMETRY TEXTURE
  glGenTextures(1, &m_GBuffer.Position);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Position);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GBuffer.Position, 0);

  // NORMALS GEOMETRY TEXTURE
  glGenTextures(1, &m_GBuffer.Normal);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Normal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GBuffer.Normal, 0);

  // DIFFUSE GEOMETRY TEXTURE
  glGenTextures(1, &m_GBuffer.Albedo);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Albedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GBuffer.Albedo, 0);

  // SURFACE GEOMETRY TEXTURE
  // r = metallic, g = IOR, b = roughness
  glGenTextures(1, &m_GBuffer.Surface);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Surface);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_GBuffer.Surface, 0);
  glDrawBuffers(4, new unsigned int[4]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 });

  // Screen quad VBO
  glGenBuffers(1, &m_ScreenVBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_ScreenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

  // Uniform buffer 
  glGenBuffers(1, &m_UniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), &m_UniformBlock, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UniformBuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateShaders()
{
  m_pShaderManager = std::make_unique<ShaderManager>();

  m_pShaderManager->AddShader("GeometryStageShader", "./Shaders/GeometryVS.glsl", "./Shaders/GeometryFS.glsl");
  m_pShaderManager->AddShader("IrradianceShader", "./Shaders/IrradianceVS.glsl", "./Shaders/IrradianceFS.glsl");
  m_pShaderManager->AddShader("PointLightShader", "./Shaders/PointLightVS.glsl", "./Shaders/PointLightFS.glsl");
  m_pShaderManager->AddShader("SunShader", "./Shaders/SunVS.glsl", "./Shaders/SunFS.glsl");

  m_pShaderManager->AddShader("SkyboxShader", "./Shaders/SkyboxVS.glsl", "./Shaders/SkyboxFS.glsl");
  m_pShaderManager->AddShader("ShadowMapShader", "./Shaders/DepthVS.glsl", "./Shaders/DepthFS.glsl");
}

void Renderer::NewFrame()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::DrawGeometry(std::vector<Model*> const& models, std::vector<std::unique_ptr<Light>> const& lights)
{
  GeometryPass(models);
  LightingPass(lights);
}

void Renderer::GeometryPass(std::vector<Model*> const& models)
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer.FBO);

  glStencilMask(0xFF);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Mark fragments that are being drawn to in the stencil buffer, so
  // only those fragments get processed in the lighting pass.
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  m_pShaderManager->UseShader("GeometryStageShader");
  m_pShaderManager->SetUniform1i("AlbedoTex", 0);
  m_pShaderManager->SetUniform1i("NormalTex", 1);
  m_pShaderManager->SetUniform1i("MetallicIORTex", 2);
  m_pShaderManager->SetUniform1i("RoughnessTex", 3);

  for (auto& model : models)
  {
    MaterialProperties const& material = model->GetMaterial()->GetProperties();
    m_pShaderManager->SetUniform1i("UseTextures", material.useTextures);

    if (!material.useTextures)
    {
      m_pShaderManager->SetUniform3f("MaterialColor", material.color);
      m_pShaderManager->SetUniform1f("MaterialMetallic", material.metallic);
      m_pShaderManager->SetUniform1f("MaterialRoughness", material.roughness);
      m_pShaderManager->SetUniform1f("MaterialIOR", material.IoR);
    }
    else
      model->GetMaterial()->Bind();

    m_pShaderManager->SetUniformMatrix("Model", model->GetTransform());
    model->Draw();
  }
}

void Renderer::LightingPass(std::vector<std::unique_ptr<Light>> const& lights)
{
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

  // Bind default buffer for drawing, bind geometry buffer textures
  // so the lighting shader can use them.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer.FBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // Copy stencil data from geometry buffer to default buffer
  glBlitFramebuffer(0, 0, m_View.WindowSize.x, m_View.WindowSize.y, 0, 0, m_View.WindowSize.x, m_View.WindowSize.y, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, m_ScreenVBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0xC, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Position);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Normal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Albedo);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_GBuffer.Surface);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_Skybox.IrradianceTexture);

  // Only draw fragments which the geometry pass has used
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilMask(0);

  // Draw ambient irradiance from skybox
  glDepthFunc(GL_ALWAYS);
  if(m_Options.DrawIndirect)
    DrawIrradiance();

  if(m_UniformBlock.debugBlock.debugMode == 0 && m_Options.DrawLights)
    DrawLights(lights);

  glDepthFunc(GL_LEQUAL);

  // Draw the skybox after everything else has been drawn
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_CULL_FACE);
  if(m_Options.DrawSkybox)
    DrawSkybox();
}

void Renderer::DrawSkybox()
{
  m_pShaderManager->UseShader("SkyboxShader");

  glBindBuffer(GL_ARRAY_BUFFER, m_Skybox.VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_Skybox.DiffuseTexture);
  glDrawArrays(GL_QUADS, 0, 24);
}

void Renderer::UpdateMatrices(glm::mat4 const& cameraTransform, float fieldOfView)
{
  if (m_View.FieldOfView != fieldOfView)
  {
    m_View.FieldOfView = fieldOfView;
    float ratio = m_View.WindowSize.x / m_View.WindowSize.y;
    m_View.ProjectionMatrix = glm::perspective(m_View.FieldOfView, ratio, 0.1f, 10000.f);
  }

  m_View.EyePosition = glm::vec3(cameraTransform[3]);
  m_View.ViewMatrix = glm::lookAt(m_View.EyePosition, m_View.EyePosition + glm::vec3(cameraTransform[2]), glm::vec3(0, 1, 0));
  
  m_UniformBlock.viewProj = m_View.ProjectionMatrix * m_View.ViewMatrix;
  m_UniformBlock.cameraPosition = m_View.EyePosition;

  glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformBlock), &m_UniformBlock);
}

void Renderer::DrawIrradiance()
{
  m_pShaderManager->UseShader("IrradianceShader");
  m_pShaderManager->SetUniform1i("PositionTex", 0);
  m_pShaderManager->SetUniform1i("NormalTex", 1);
  m_pShaderManager->SetUniform1i("AlbedoTex", 2);
  m_pShaderManager->SetUniform1i("SurfaceTex", 3);
  m_pShaderManager->SetUniform1i("IrradianceTex", 4);
  m_pShaderManager->SetUniform1i("BRDFLutTex", 5);
  m_pShaderManager->SetUniform1i("PrefilteredTex[0]", 6);
  m_pShaderManager->SetUniform1i("PrefilteredTex[1]", 7);
  m_pShaderManager->SetUniform1i("PrefilteredTex[2]", 8);
  m_pShaderManager->SetUniform1i("PrefilteredTex[3]", 9);
  m_pShaderManager->SetUniform1i("PrefilteredTex[4]", 10);
  m_pShaderManager->SetUniform1i("PrefilteredTex[5]", 11);
  m_pShaderManager->SetUniform1i("PrefilteredTex[6]", 12);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, m_Skybox.BRDFlut);
  for (int i = 0; i < 7; ++i)
  {
    glActiveTexture(GL_TEXTURE6 + i);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Skybox.PrefilteredTexture[i]);
  }

  glDrawArrays(GL_QUADS, 0, 4);
}

void Renderer::DrawLights(std::vector<std::unique_ptr<Light>> const& lights)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  if (m_Options.DrawSun)
  {
    m_pShaderManager->UseShader("SunShader");
    m_pShaderManager->SetUniform1i("PositionTex", 0);
    m_pShaderManager->SetUniform1i("NormalTex", 1);
    m_pShaderManager->SetUniform1i("AlbedoTex", 2);
    m_pShaderManager->SetUniform1i("SurfaceTex", 3);
    glDrawArrays(GL_QUADS, 0, 4);
  }

  m_pShaderManager->UseShader("PointLightShader");
  m_pShaderManager->SetUniform1i("PositionTex", 0);
  m_pShaderManager->SetUniform1i("NormalTex", 1);
  m_pShaderManager->SetUniform1i("AlbedoTex", 2);
  m_pShaderManager->SetUniform1i("SurfaceTex", 3);

  for (auto& pointLight : lights)
  {
    m_pShaderManager->SetUniform3f("lightPosition", pointLight->GetPosition());
    m_pShaderManager->SetUniform3f("lightColor", pointLight->GetColor());
    m_pShaderManager->SetUniform1f("lightRadius", pointLight->GetRadius());
    glDrawArrays(GL_QUADS, 0, 4);
  }

  glDisable(GL_BLEND);
}