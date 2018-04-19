#define GLEW_STATIC

#include "Camera.h"
#include "ModelManager.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

static const glm::vec2  g_initialSize(1280, 720);
static const char*      g_windowTitle = "BRDF Example";
static       bool       g_showUI  = false;

static       int        g_method = 0;

static const char*      methods = "Final\0Position\0Normal\0Diffuse\0Metallic\0Roughness";

static void errorCallback(int error, const char* description)
{
  printf("Error: %s\n", description);
}

static void windowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
  printf("glfwWindowSizeCallback %d %d\n", width, height);
}

static void drawUI()
{
  ImGui_ImplGlfwGL3_NewFrame();
  ImGui::Begin("Test");
  {
    ImGui::Text("Hello world");
    ImGui::Combo("Draw method", &g_method, methods);
  }ImGui::End();
  ImGui::Render();
  ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

int main(int argc, char* argv[])
{
  glfwSetErrorCallback(errorCallback);
  if (!glfwInit())
  {
    printf("Failed to initialize GLFW\n");
    return 0;
  }

  GLFWwindow* window = glfwCreateWindow(g_initialSize.x, g_initialSize.y, g_windowTitle, NULL, NULL);
  if (!window)
  {
    printf("Failed to create a window\n");
    return 0;
  }
  glfwSetWindowSizeCallback(window, windowSizeCallback);
  glfwMakeContextCurrent(window);

  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    printf("Failed to initialize GLEW\n");
    printf("Error: %s\n", glewGetErrorString(err));
    return 0;
  }

  ImGui::CreateContext();
  ImGui_ImplGlfwGL3_Init(window, true);
  ImGui::StyleColorsDark();

  //glEnableClientState(GL_VERTEX_ARRAY);

  Camera camera;
  ModelManager modelManager;
  Renderer renderer;
  
  renderer.Initialize(g_initialSize.x, g_initialSize.y);
  modelManager.LoadObj("./Resources/Scene.obj");

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1.0f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  double lastFrame = glfwGetTime();

  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    
    double dt = glfwGetTime() - lastFrame;;
    lastFrame = glfwGetTime();

    camera.Update(dt);
    renderer.UpdateViewMatrix(camera.GetViewMatrix());
    renderer.UpdateViewPos(camera.GetPosition());

    // Rendering passes
    {
      renderer.NewFrame();

      renderer.SetupGeometryPass();
      modelManager.Draw();

      renderer.SetupLightingPass(g_method);

      //TODO: Implement light manager for multiple light sources
      //lightManager.DrawLights()
    }


    drawUI();

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}