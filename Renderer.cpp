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

Renderer::Renderer() : 
  m_fieldOfView(glm::radians(50.f))
{

}

Renderer::~Renderer()
{

}

void Renderer::Initialize(glm::vec2 const& initialSize)
{
  m_WindowSize = initialSize;

  m_projMatrix = glm::perspective(m_fieldOfView, initialSize.x/initialSize.y, 0.1f, 10000.f);
  CreateBuffers(initialSize.x, initialSize.y);
  CreateShaders();

  skybox = SOIL_load_OGL_cubemap("./Resources/Textures/miramar/miramar_lf.tga",
    "./Resources/Textures/miramar/miramar_rt.tga",
    "./Resources/Textures/miramar/miramar_up.tga",
    "./Resources/Textures/miramar/miramar_dn.tga",
    "./Resources/Textures/miramar/miramar_ft.tga",
    "./Resources/Textures/miramar/miramar_bk.tga",
    SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);
  
  skyboxIrradiance = SOIL_load_OGL_cubemap("./Resources/Textures/miramar/miramar_radiance_lf.png",
    "./Resources/Textures/miramar/miramar_radiance_rt.png",
    "./Resources/Textures/miramar/miramar_radiance_up.png",
    "./Resources/Textures/miramar/miramar_dn.tga",
    "./Resources/Textures/miramar/miramar_radiance_ft.png",
    "./Resources/Textures/miramar/miramar_radiance_bk.png",
    SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);


  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIrradiance);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

  glm::vec3 lightDir = glm::normalize(glm::vec3(1.7, -1, 1));

  m_DirectionalLight.projMatrix = glm::ortho<float>(-15, 15, -15, 15, -30, 30);
  m_DirectionalLight.viewMatrix = glm::lookAt(glm::vec3(0, 0, 0), lightDir, glm::vec3(0, 1, 0));
  m_UpdateShadowMaps = true;
}

void Renderer::CreateBuffers(int width, int height)
{
  // Create frame buffer and geometry textures for deferred rendering
  // https://learnopengl.com/Advanced-Lighting/Deferred-Shading

  // At the moment the textures are named based on the PBR metallic/roughness
  // workflow, first goal is to get Cook-Torrance working. 
  // The ultimate goal is to be able compare BRDF functions/workflows
  // so metallic might store glossiness etc.

  // Depth Buffer and texture for shadow mapping
  glGenFramebuffers(1, &DepthFBO);
  glGenTextures(1, &m_DirectionalLight.shadowMap);
  glBindTexture(GL_TEXTURE_2D, m_DirectionalLight.shadowMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindFramebuffer(GL_FRAMEBUFFER, DepthFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DirectionalLight.shadowMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glGenFramebuffers(1, &m_gbuffer.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);

  // DEPTH-STENCIL TEXTURE
  glGenTextures(1, &m_gbuffer.depth);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer.depth, 0);

  // POSITION GEOMETRY TEXTURE
  glGenTextures(1, &m_gbuffer.position);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.position);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gbuffer.position, 0);

  // NORMALS GEOMETRY TEXTURE
  glGenTextures(1, &m_gbuffer.normals);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.normals);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gbuffer.normals, 0);

  // DIFFUSE-METALLIC GEOMETRY TEXTURE
  // Diffuse = rgb, Metallic = a
  glGenTextures(1, &m_gbuffer.albedoMetallic);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.albedoMetallic);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gbuffer.albedoMetallic, 0);

  // ROUGHNESS GEOMETRY TEXTURE
  glGenTextures(1, &m_gbuffer.roughness);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.roughness);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gbuffer.roughness, 0);

  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(4, attachments);

  // Screen quad VBO
  glGenBuffers(1, &m_screenVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

  // Uniform buffer 
  glGenBuffers(1, &m_uniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), &m_uniformBlock, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CreateShaders()
{
  m_shaderManager.AddShader("GeometryStageShader", "./Shaders/GeometryVS.glsl", "./Shaders/GeometryFS.glsl");
  m_shaderManager.AddShader("LightingStageShader", "./Shaders/LightingVS.glsl", "./Shaders/LightingFS.glsl");
  m_shaderManager.AddShader("IrradianceShader", "./Shaders/IrradianceVS.glsl", "./Shaders/IrradianceFS.glsl");
  m_shaderManager.AddShader("SkyboxShader", "./Shaders/SkyboxVS.glsl", "./Shaders/SkyboxFS.glsl");
  m_shaderManager.AddShader("ShadowMapShader", "./Shaders/DepthVS.glsl", "./Shaders/DepthFS.glsl");
}

void Renderer::NewFrame()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::GeometryPass()
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  if (m_UpdateShadowMaps)
  {
    UpdateShadowMap();
    return;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);

  glStencilMask(0xFF);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Mark fragments which should be drawn
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  m_shaderManager.UseShader("GeometryStageShader");
  m_shaderManager.SetUniform1i("texture_Diffuse", 0);
  m_shaderManager.SetUniform1i("texture_Normal", 1);
  m_shaderManager.SetUniform1i("texture_Metal", 2);
  m_shaderManager.SetUniform1i("texture_Roughness", 3);
}

void Renderer::LightingPass(int brdf, int debug)
{
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

  if (m_UpdateShadowMaps)
  {
    m_UpdateShadowMaps = false;
    glViewport(0, 0, 1280, 720);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(GL_NONE);
    return;
  }

  // Bind back-buffer for drawing, bind gbuffer textures
  // so the lighting shader can use them.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // Copy stencil data from frame buffer to default buffer
  glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0xC, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.position);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.normals);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.albedoMetallic);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.roughness);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxIrradiance);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, m_DirectionalLight.shadowMap);

  // Only draw fragments which the geometry pass has used
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilMask(0);

  // Draw ambient irradiance from skybox
 // glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);
  DrawIrradiance(debug);
  if(debug == 0)
    DrawLights(brdf);
  glDepthFunc(GL_LEQUAL);

  // Draw the skybox after everything else has been drawn
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_CULL_FACE);
  DrawSkybox();
}

// Obsolete for now. Implement HDR buffer 
void Renderer::Present()
{
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
}

void Renderer::DrawSkybox()
{
  m_shaderManager.UseShader("SkyboxShader");

  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
  glDrawArrays(GL_QUADS, 0, 24);
}

void Renderer::UpdateMatrices(glm::mat4 const& cameraTransform, float fieldOfView)
{
  if (m_fieldOfView != fieldOfView)
  {
    m_fieldOfView = fieldOfView;
    m_projMatrix = glm::perspective(m_fieldOfView, m_WindowSize.x/m_WindowSize.y, 0.1f, 10000.f);
  }

  m_viewPos = glm::vec3(cameraTransform[3]);
  m_viewMatrix = glm::lookAt(m_viewPos, m_viewPos + glm::vec3(cameraTransform[2]), glm::vec3(0, 1, 0));
  
  m_uniformBlock.modelViewProj = m_projMatrix * m_viewMatrix;
  m_uniformBlock.cameraPosition = m_viewPos;

  // Update shader uniform block
  glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
  UniformBlock* pBlock = (UniformBlock*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(UniformBlock), GL_MAP_WRITE_BIT);
  *pBlock = m_uniformBlock;
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void Renderer::UpdateShadowMap()
{
  glViewport(0, 0, 4096, 4096);
  glBindFramebuffer(GL_FRAMEBUFFER, DepthFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glm::mat4 LightMVP = m_DirectionalLight.projMatrix * m_DirectionalLight.viewMatrix;

  m_shaderManager.UseShader("ShadowMapShader");
  m_shaderManager.SetUniformMatrix("LightMVP", LightMVP);
}

void Renderer::DrawIrradiance(int debugMethod)
{
  m_shaderManager.UseShader("IrradianceShader");
  m_shaderManager.SetUniform1i("texture_position", 0);
  m_shaderManager.SetUniform1i("texture_normal", 1);
  m_shaderManager.SetUniform1i("texture_albedoMetal", 2);
  m_shaderManager.SetUniform1i("texture_roughness", 3);
  m_shaderManager.SetUniform1i("skyboxTexture", 4);
  m_shaderManager.SetUniform1i("method", debugMethod);

  glDrawArrays(GL_QUADS, 0, 4);
}

void Renderer::DrawLights(int brdfMethod)
{
  glm::mat4 LightMVP = m_DirectionalLight.projMatrix * m_DirectionalLight.viewMatrix;

  m_shaderManager.UseShader("LightingStageShader");
  m_shaderManager.SetUniform1i("texture_position", 0);
  m_shaderManager.SetUniform1i("texture_normal", 1);
  m_shaderManager.SetUniform1i("texture_albedoMetal", 2);
  m_shaderManager.SetUniform1i("texture_roughness", 3);
  m_shaderManager.SetUniform1i("shadowMap", 5);
  m_shaderManager.SetUniform1i("brdfMethod", brdfMethod);
  m_shaderManager.SetUniformMatrix("LightMVP", LightMVP);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glDrawArrays(GL_QUADS, 0, 4);
  glDisable(GL_BLEND);
}
