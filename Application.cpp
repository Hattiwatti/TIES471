#define GLEW_STATIC

#include "Application.h"

#include <iostream>
#include <thread>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

static const glm::vec2  g_initialSize(1280, 720);
static const char*      g_windowTitle = "BRDF Example";
static       bool       g_showUI = true;
static       bool       g_hasFocus = true;
static       int        g_method = 0;

static const char*     methods[] =
{ "Final", "Position", "Normal", "Diffuse", "Metallic", "Roughness", "Irradiance", "Shadowmap" };

Application::Application() :
  m_LastFrameTime(0),
  m_pCamera(nullptr),
  m_pModelManager(nullptr),
  m_pRenderer(nullptr),
  m_pWindow(nullptr)
{

}

Application::~Application()
{

}

bool Application::Initialize()
{
  /*--------------------------------------
    Initialize GLFW and create a window
   ---------------------------------------*/
  glfwSetErrorCallback(errorCallback);
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  m_pWindow = glfwCreateWindow(g_initialSize.x, g_initialSize.y, g_windowTitle, NULL, NULL);
  if (!m_pWindow)
  {
    std::cerr << "Failed to create a window" << std::endl;
    return false;
  }

  glfwMakeContextCurrent(m_pWindow);

  glfwSetWindowFocusCallback(m_pWindow, focusCallback);
  glfwSetWindowSizeCallback(m_pWindow, sizeCallback);

  /*------------------
    Initialize GLEW
   -------------------*/
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    printf("Error: %s\n", glewGetErrorString(err));
    return false;
  }

  /*------------------
    Initialize ImGui
   -------------------*/

  ImGui::CreateContext();
  ImGui_ImplGlfwGL3_Init(m_pWindow, true);
  ImGui::StyleColorsDark();

  /*-----------------------------------------*/

  m_pCamera = std::make_unique<Camera>();
  m_pModelManager = std::make_unique<ModelManager>();
  m_pRenderer = std::make_unique<Renderer>();

  m_pRenderer->Initialize(g_initialSize);
  m_pModelManager->LoadObj("./Resources/SceneNew.obj");

  return true;
}

void Application::Run()
{
  // For some reason key callback refuses to work
  // if it's set in Initialize()
  glfwSetKeyCallback(m_pWindow, keyCallback);

  while (!glfwWindowShouldClose(m_pWindow))
  {
    glfwPollEvents();

    m_CurrentFrameTime = glfwGetTime();
    m_dtFrameTime = m_CurrentFrameTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentFrameTime;

    m_pCamera->Update(m_dtFrameTime);
    m_pRenderer->UpdateMatrices(m_pCamera->GetTransform(), m_pCamera->GetFov());

    m_pRenderer->NewFrame();

    m_pRenderer->GeometryPass();
    m_pModelManager->Draw();

    m_pRenderer->LightingPass(g_method);
    //m_pRenderer->Present();

    if (g_showUI)
    {
      ImGui_ImplGlfwGL3_NewFrame();
      ImGui::Begin("Test");
      {
        ImGui::Text("Hello world");
        ImGui::Combo("Draw method", &g_method, methods, 8);
        if (ImGui::Button("Recompile shaders"))
          m_pRenderer->RecompileShaders();
      }ImGui::End();
      ImGui::Render();
      ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(m_pWindow);
  }
}

/*--------------------
    GLFW Callbacks
 ---------------------*/

void Application::errorCallback(int error, const char* description)
{
  printf("ErrorCallback Error %d, description: %s\n", error, description);
}

void Application::focusCallback(GLFWwindow* pWindow, int focused)
{
  printf("FocusCallback Window 0x%X\tFocused: %d\n", pWindow, focused);
}

void Application::sizeCallback(GLFWwindow* pWindow, int width, int height)
{
  printf("SizeCallback Window 0x%X\tSize %dx%d\n", pWindow, width, height);
}

void Application::keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
  printf("KeyCallback Window 0x%X\tKey %d, Scancode %d, Action %d, Mods %d\n", pWindow, key, scancode, action, mods);
}

/*---------------------------------------------------------------------------*/