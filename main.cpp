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
static       bool       g_showUI = false;
static       bool       g_hasFocus = true;

static       int        g_method = 0;

static const char*      methods = "Final\0Position\0Normal\0Diffuse\0Metallic\0Roughness\0Irradiance\0ShadowMap\0";

static void errorCallback(int error, const char* description)
{
  printf("Error: %s\n", description);
}

static void windowFocusCallback(GLFWwindow* window, int focused)
{
  g_hasFocus = (focused == GLFW_TRUE);
}

static void windowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
  printf("glfwWindowSizeCallback %d %d\n", width, height);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_E && action == GLFW_PRESS)
  {
    printf("aaaa");
  }
}
int main(int argc, char* argv[])
{
  glfwSetErrorCallback(errorCallback);
  if (!glfwInit())
  {
    std::cerr << ("Failed to initialize GLFW\n");
    return 0;
  }

  GLFWwindow* window = glfwCreateWindow(g_initialSize.x, g_initialSize.y, g_windowTitle, NULL, NULL);
  if (!window)
  {
    printf("Failed to create a window\n");
    return 0;
  }

  glfwSetWindowFocusCallback(window, windowFocusCallback);
  glfwSetWindowSizeCallback(window, windowSizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, g_showUI ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
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
  modelManager.LoadObj("./Resources/SceneNew.obj");

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glDepthFunc(GL_LESS);
  glClearDepth(1.0f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  double lastFrame = glfwGetTime();
  double mouseX = 0, mouseY = 0;
  glfwGetCursorPos(window, &mouseX, &mouseY);
  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    double dt = glfwGetTime() - lastFrame;
    lastFrame = glfwGetTime();


    // Update camera input and send transform and field of view
    // to renderer so matrices can be updated.

    if (g_hasFocus)
    {
      if (GetAsyncKeyState(VK_INSERT) & 0x8000)
      {
        g_showUI = !g_showUI;
        glfwSetInputMode(window, GLFW_CURSOR, g_showUI ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        while (GetAsyncKeyState(VK_INSERT) & 0x8000)
          Sleep(100);
      }
      camera.Update(dt);
    }


    renderer.UpdateMatrices(camera.GetTransform(), camera.GetFov());

    // Rendering passes
    {
      renderer.NewFrame();

      renderer.GeometryPass();
      modelManager.Draw();

      renderer.LightingPass(g_method);

      //TODO: Maybe implement light manager for multiple light sources
      //lightManager.DrawLights()
    }

    if (g_showUI)
    {
      ImGui_ImplGlfwGL3_NewFrame();
      ImGui::Begin("Test");
      {
        ImGui::Text("Hello world");
        ImGui::Combo("Draw method", &g_method, methods);
        if (ImGui::Button("Recompile shaders"))
          renderer.RecompileShaders();
      }ImGui::End();
      ImGui::Render();
      ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}