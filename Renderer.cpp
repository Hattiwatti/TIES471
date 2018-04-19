#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

static const glm::vec3 g_ScreenQuad[4] =
{
  glm::vec3(-1.f, -1.f, 0),
  glm::vec3(1.f,  -1.f, 0),
  glm::vec3(1.f,  1.f, 0),
  glm::vec3(-1.f, 1.f, 0)
};

static glm::mat4 g_projection = glm::perspective(glm::radians(80.f), 1.7777f, 0.1f, 10000.f);
static const glm::mat4 g_view = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Initialize(int width, int height)
{
  g_projection = glm::perspective(glm::radians(50.f), (float)width/(float)height, 0.1f, 10000.f);
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

  glGenTextures(1, &m_gbuffer.depth);
  glBindTexture(GL_TEXTURE_2D, m_gbuffer.depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer.depth, 0);

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
}

void Renderer::NewFrame()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearDepth(1.0f);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetupGeometryPass()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer.fbo);
  glClearColor(0, 0, 0, 1);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_gbuffer.shader->Bind();

  GLuint textureLocation = glGetUniformLocation(m_gbuffer.shader->GetID(), "texture_Diffuse");
  glUniform1i(textureLocation, 0);
  textureLocation = glGetUniformLocation(m_gbuffer.shader->GetID(), "texture_Normal");
  glUniform1i(textureLocation, 1);
  textureLocation = glGetUniformLocation(m_gbuffer.shader->GetID(), "texture_Metal");
  glUniform1i(textureLocation, 2);
  textureLocation = glGetUniformLocation(m_gbuffer.shader->GetID(), "texture_Roughness");
  glUniform1i(textureLocation, 3);

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

void Renderer::SetupLightingPass(int method)
{
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(3);

  m_screenShader->Bind();

  // Bind back-buffer for drawing, bind gbuffer textures
  // so the lighting shader can use them.

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

  GLuint textureLocation = glGetUniformLocation(m_screenShader->GetID(), "texture_albedoMetal");
  glUniform1i(textureLocation, 2);
  textureLocation = glGetUniformLocation(m_screenShader->GetID(), "texture_position");
  glUniform1i(textureLocation, 0);
  textureLocation = glGetUniformLocation(m_screenShader->GetID(), "texture_normal");
  glUniform1i(textureLocation, 1);
  textureLocation = glGetUniformLocation(m_screenShader->GetID(), "texture_roughness");
  glUniform1i(textureLocation, 3);

  GLuint methodLocation = glGetUniformLocation(m_screenShader->GetID(), "method");
  glUniform1i(methodLocation, method);
  
  GLuint viewPosLocation = glGetUniformLocation(m_screenShader->GetID(), "viewPos");
  glUniform3fv(viewPosLocation, 1, &m_viewPos[0]);

  // Draw full screen squad to evaluate gbuffer and draw final image.
  // Optionally implement a light manager and draw lighting volumes
  // so only fragments illuminated by the lights are processed
  glDrawArrays(GL_QUADS, 0, 4);
}

// Probably obsolete
void Renderer::Present()
{
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, m_screenVbo);
}