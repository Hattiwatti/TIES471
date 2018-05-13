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
static       int        g_debugMode = 0;
static       int        g_brdfMethod = 1;

static const char*     debugModes[] =
{ "Final", "Position", "Normal", "Diffuse", "Metallic", "Roughness", "Irradiance", "Shadowmap" };

static const char*     brdfMethods[] =
{ "Blinn-Phong", "Cook-Torrance" };

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
    abort();
    return false;
  }

  m_pWindow = glfwCreateWindow(g_initialSize.x, g_initialSize.y, g_windowTitle, NULL, NULL);
  if (!m_pWindow)
  {
    std::cerr << "Failed to create a window" << std::endl;
    abort();
    return false;
  }

  glfwMakeContextCurrent(m_pWindow);

  glfwSetWindowFocusCallback(m_pWindow, focusCallback);
  glfwSetWindowSizeCallback(m_pWindow, sizeCallback);

  glfwSwapInterval(0);

  /*------------------
    Initialize GLEW
   -------------------*/
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    printf("Error: %s\n", glewGetErrorString(err));
    abort();
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

  m_pModelManager->CreateSphereGrid();

  return true;
}

void Application::Run()
{
  // For some reason key callback refuses to work if it's set in Initialize()
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
    m_pRenderer->GeometryPass(m_pModelManager->GetModels());
    m_pRenderer->LightingPass(g_brdfMethod, g_debugMode);

    //m_pRenderer->Present();

    if (g_showUI)
    {
      ImGui_ImplGlfwGL3_NewFrame();

      ImGui::Begin("Test");
      {
        ImGui::Text("Hello world");

        ImGui::Text("Debug mode");
        ImGui::Combo("##debug", &g_debugMode, debugModes, 8);

        ImGui::Text("BRDF Methods");
        ImGui::Combo("##BRDFMethod", &g_brdfMethod, brdfMethods, 2);

        DebugUniformBlock& debug = m_pRenderer->GetDebugStruct();
        ImGui::Text("Albeido");
        ImGui::InputFloat("##Albeido", &debug.AlbeidoMultiplier, 0.01f, 0.01f, 2);
        ImGui::Text("Metallic");
        ImGui::InputFloat("##Metallic", &debug.MetallicMultiplier, 0.01f, 0.01f, 2);
        ImGui::Text("Roughness");
        ImGui::InputFloat("##Roughness", &debug.RoughnessMultiplier, 0.01f, 0.01f, 2);

        debug.AlbeidoMultiplier = max(0, debug.AlbeidoMultiplier);
        debug.MetallicMultiplier = max(0, debug.MetallicMultiplier);
        debug.RoughnessMultiplier = max(0, debug.RoughnessMultiplier);

        if (ImGui::Button("Recompile shaders"))
          m_pRenderer->RecompileShaders();

        ImGui::Dummy(ImVec2(10, 10));
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::PlotLines("##FPS2", m_FPSBuffer, 99, 0, "Frame time in milliseconds", 0, 2, ImVec2(250, 100));

        ImGui::Dummy(ImVec2(10, 10));

        m_FPSBuffer[99] += (m_dtFrameTime) / 120;
        m_FPSSamples += 1;
        if (m_FPSSamples >= 120)
        {
          m_FPSSamples = 0;
          m_FPSBuffer[99] *= 1000;
          for (int i = 0; i < 99; ++i)
            m_FPSBuffer[i] = m_FPSBuffer[i + 1];
          m_FPSBuffer[99] = 0;
        }

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