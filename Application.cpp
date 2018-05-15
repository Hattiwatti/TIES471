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

static const glm::vec2  g_initialSize(1920, 1080);
static const char*      g_windowTitle = "BRDF Example";
static       bool       g_showUI = true;
static       bool       g_hasFocus = true;

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
  m_pLightManager = std::make_unique<LightManager>();
  m_pModelManager = std::make_unique<ModelManager>();
  m_pRenderer = std::make_unique<Renderer>();

  m_pRenderer->Initialize(g_initialSize);
  m_pModelManager->LoadObj("./Resources/Scene.obj");

  m_pModelManager->CreateSphereGrid();

  return true;
}

void Application::ThreadingTest()
{
  while (true)
  {
    if (g_hasFocus)
    {
      m_pCamera->Update(0.03);
      m_pRenderer->UpdateMatrices(m_pCamera->GetTransform(), m_pCamera->GetFov());
    }

    Sleep(30);
  }
}

void Application::Run()
{
  // For some reason key callback refuses to work if it's set in Initialize()
  glfwSetKeyCallback(m_pWindow, keyCallback);

  //std::thread th1(&Application::ThreadingTest, this);
  //th1.detach();

  while (!glfwWindowShouldClose(m_pWindow))
  {
    glfwPollEvents();

    m_CurrentFrameTime = glfwGetTime();
    m_dtFrameTime = m_CurrentFrameTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentFrameTime;

    if (g_hasFocus)
    {
      // Update camera and render view accordingly

      m_pCamera->Update(m_dtFrameTime);
      m_pRenderer->UpdateMatrices(m_pCamera->GetTransform(), m_pCamera->GetFov());
    }

    // Update randomly moving point lights
    m_pLightManager->Update(m_dtFrameTime);

    // Draw stuff
    m_pRenderer->NewFrame();
    m_pRenderer->DrawGeometry(m_pModelManager->GetModels(), m_pLightManager->GetLights());

    // UI drawing
    if (g_showUI)
    {
      ImGui_ImplGlfwGL3_NewFrame();

      ImGui::Begin("Test");
      {
        ImGui::Text("Hello world");
        DebugUniformBlock& debug = m_pRenderer->GetDebugStruct();
        RenderOptions& options = m_pRenderer->GetOptions();

        ImGui::Text("Debug mode");
        ImGui::Combo("##debug", &debug.debugMode, debugModes, 8);

        ImGui::Text("BRDF Methods");
        ImGui::Combo("##BRDFMethod", &debug.brdfMethod, brdfMethods, 2);

        ImGui::Text("Albeido");
        ImGui::InputFloat("##Albeido", &debug.AlbeidoMultiplier, 0.01f, 0.01f, 2);
        ImGui::Text("Metallic");
        ImGui::InputFloat("##Metallic", &debug.MetallicMultiplier, 0.01f, 0.01f, 2);
        ImGui::Text("Roughness");
        ImGui::InputFloat("##Roughness", &debug.RoughnessMultiplier, 0.01f, 0.01f, 2);
        ImGui::Text("Specular shininess");
        ImGui::InputFloat("##Shininess", &debug.HardcodedSpecular, 1.f, 2.f, 2);

        debug.AlbeidoMultiplier = max(0, debug.AlbeidoMultiplier);
        debug.MetallicMultiplier = max(0, debug.MetallicMultiplier);
        debug.RoughnessMultiplier = max(0, debug.RoughnessMultiplier);

        ImGui::Checkbox("Draw sunlight", &options.DrawSun);
        ImGui::Checkbox("Draw skybox", &options.DrawSkybox);
        ImGui::Checkbox("Draw lights", &options.DrawLights);
        ImGui::Checkbox("Draw irradiance", &options.DrawIndirect);

        if (ImGui::Button("Recompile shaders"))
          m_pRenderer->RecompileShaders();

        if (ImGui::Button("Create Lights"))
          m_pLightManager->CreateLights();
        ImGui::SameLine();
        if (ImGui::Button("Destroy Lights"))
          m_pLightManager->DestroyLights();

        ImGui::Dummy(ImVec2(10, 10));
        ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
        ImGui::PlotLines("##FPS2", m_FPSBuffer, 99, 0, "Frame time (ms)", 0, 5, ImVec2(250, 100));

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
  g_hasFocus = focused;
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