#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

static const glm::vec3 g_ScreenQuad[4] =
{
  glm::vec3(-1.f, -1.f, 0),
  glm::vec3(-1.f,  1.f, 0),
  glm::vec3(1.f,  1.f, 0),
  glm::vec3(1.f, -1.f, 0)
};

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Initialize(int width, int height)
{
  CreateBuffers(width, height);

  m_screenShader = new Shader("./Shaders/ScreenVertex.glsl", "./Shaders/ScreenFrag.glsl");
  m_gbuffer.shader = new Shader("./Shaders/GeometryVertex.glsl", "./Shaders/GeometryFrag.glsl");
}

void Renderer::CreateBuffers(int width, int height)
{
  // Create frame buffer and geometry textures for deferred rendering
  // https://learnopengl.com/Advanced-Lighting/Deferred-Shading

  glGenFramebuffers(1, &m_gbuffer.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, width, height, 0, GL_R, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gbuffer.roughness, 0);

  glGenBuffers(1, &m_screenVbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_ScreenQuad), g_ScreenQuad, GL_STATIC_DRAW);
}

void Renderer::NewFrame()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetupGeometryPass()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_gbuffer.shader->Bind();
}