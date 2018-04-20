#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <SOIL/SOIL.h>

static const glm::vec3 g_ScreenQuad[4] =
{
  glm::vec3(-1.f, -1.f, 0),
  glm::vec3(1.f,  -1.f, 0),
  glm::vec3(1.f,  1.f, 0),
  glm::vec3(-1.f, 1.f, 0)
};
static glm::mat4 g_projection = glm::perspective(glm::radians(80.f), 1.7777f, 0.1f, 10000.f);

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

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Initialize(int width, int height)
{
  g_projection = glm::perspective(glm::radians(50.f), (float)width / (float)height, 0.1f, 10000.f);
  CreateBuffers(width, height);
  CreateShaders();

  skybox = SOIL_load_OGL_cubemap("./Resources/Textures/miramar/miramar_lf.tga",
    "./Resources/Textures/miramar/miramar_rt.tga",
    "./Resources/Textures/miramar/miramar_up.tga",
    "./Resources/Textures/miramar/miramar_dn.tga",
    "./Resources/Textures/miramar/miramar_ft.tga",
    "./Resources/Textures/miramar/miramar_bk.tga",
    SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB);
  
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
}

void Renderer::CreateBuffers(int width, int height)
{
  // Create frame buffer and geometry textures for deferred rendering
  // https://learnopengl.com/Advanced-Lighting/Deferred-Shading

  glGenFramebuffers(1, &m_gbuffer.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);

  glGenTextures(1, &m_gbuffer.depth);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer.depth, 0);

  glGenTextures(1, &m_gbuffer.position);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.position);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gbuffer.position, 0);

  glGenTextures(1, &m_gbuffer.normals);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.normals);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gbuffer.normals, 0);

  // Diffuse color is combined with metallic values.
  // RGB = diffuse, A = metallic
  glGenTextures(1, &m_gbuffer.albedoMetallic);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.albedoMetallic);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gbuffer.albedoMetallic, 0);

  glGenTextures(1, &m_gbuffer.roughness);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.roughness);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gbuffer.roughness, 0);

  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(4, attachments);

  //glGenVertexArrays(1, &m_screenVao);
  //glBindVertexArray(m_screenVao);

  glGenBuffers(1, &m_screenVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_ScreenQuad), g_ScreenQuad, GL_STATIC_DRAW);

  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0xC, 0);
  //glEnableVertexAttribArray(0);
  //glBindVertexArray(0);

  glGenBuffers(1, &m_uniformBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBlock), &m_uniformBlock, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer.depth, 0);
}

void Renderer::CreateShaders()
{
  m_shaderManager.AddShader("GeometryStageShader", "./Shaders/GeometryVS.glsl", "./Shaders/GeometryFS.glsl");
  m_shaderManager.AddShader("LightingStageShader", "./Shaders/LightingVS.glsl", "./Shaders/LightingFS.glsl");
  m_shaderManager.AddShader("SkyboxShader", "./Shaders/SkyboxVS.glsl", "./Shaders/SkyboxFS.glsl");
}

void Renderer::NewFrame()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearDepth(1.0f);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::GeometryPass()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);
  glClearColor(0, 0, 0, 1);
  glStencilMask(0xFF);
  glClearDepth(1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  m_shaderManager.UseShader("GeometryStageShader");
  m_shaderManager.SetUniform1i("texture_Diffuse", 0);
  m_shaderManager.SetUniform1i("texture_Normal", 1);
  m_shaderManager.SetUniform1i("texture_Metal", 2);
  m_shaderManager.SetUniform1i("texture_Roughness", 3);

  // Model matrix is not really needed since a single .obj
  // is used as the whole scene, so vertices are already in
  // world space.
  glm::mat4 modelViewProj = g_projection * m_viewMatrix;
  glm::mat3 normalMatrix = glm::mat3( glm::transpose( glm::inverse(m_viewMatrix) ) );
  glUniformMatrix4fv(0, 1, GL_FALSE, &modelViewProj[0][0]);
  glUniformMatrix3fv(1, 1, GL_FALSE, &normalMatrix[0][0]);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
}

void Renderer::LightingPass(int method)
{
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

  // Bind back-buffer for drawing, bind gbuffer textures
  // so the lighting shader can use them.

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0xC, 0);
  glEnableVertexAttribArray(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.position);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.normals);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.albedoMetallic);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.roughness);

  m_shaderManager.UseShader("LightingStageShader");
  m_shaderManager.SetUniform1i("texture_position", 0);
  m_shaderManager.SetUniform1i("texture_normal", 1);
  m_shaderManager.SetUniform1i("texture_albedoMetal", 2);
  m_shaderManager.SetUniform1i("texture_roughness", 3);
  m_shaderManager.SetUniform1i("method", method);

  // Only draw fragments which the geometry pass has used
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilMask(0);

  // Draw full screen quad to evaluate gbuffer and draw final image.
  // Optionally implement a light manager and draw lighting volumes
  // so only fragments illuminated by the lights are processed
  glDrawArrays(GL_QUADS, 0, 4);

  glDisable(GL_STENCIL_TEST);
  glDisable(GL_CULL_FACE);
  DrawSkybox();
}

// Probably obsolete
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

void Renderer::UpdateMatrices(glm::mat4 const& cameraTransform)
{
  m_viewPos = glm::vec3(cameraTransform[3]);
  m_viewMatrix = glm::lookAt(m_viewPos, m_viewPos + glm::vec3(cameraTransform[2]), glm::vec3(0, 1, 0));
  
  m_uniformBlock.modelViewProj = g_projection * m_viewMatrix;
  m_uniformBlock.cameraPosition = m_viewPos;

  glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
  UniformBlock* pBlock = (UniformBlock*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(UniformBlock), GL_MAP_WRITE_BIT);
  *pBlock = m_uniformBlock;
  glUnmapBuffer(GL_UNIFORM_BUFFER);
}