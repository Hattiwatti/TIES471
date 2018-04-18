#define GLEW_STATIC

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

  Renderer renderer;
  renderer.Initialize(g_initialSize.x, g_initialSize.y);

  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    //renderer->NewFrame();

    //renderer->SetupGPass();
    //modelManager->Draw();
    //renderer->SetupLightingPass();
    //modelManager->Draw();
    //renderer->Present();

    if (g_showUI)
      drawUI();

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}