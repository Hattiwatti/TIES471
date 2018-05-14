#pragma once
#include "Camera.h"
#include "Renderer.h"
#include "LightManager.h"
#include "ModelManager.h"

#include <GLFW/glfw3.h>
#include <memory>

class Application
{
public:
  Application();
  ~Application();

  bool Initialize();
  void Run();

private:
  static void errorCallback(int error, const char* description);
  static void focusCallback(GLFWwindow* pWindow, int focused);
  static void sizeCallback(GLFWwindow* pWindow, int width, int height);
  static void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);

  void ThreadingTest();

private:
  double m_CurrentFrameTime;
  double m_LastFrameTime;
  double m_dtFrameTime;

  int m_FPSSamples;
  float m_FPSBuffer[100];

  std::unique_ptr<Camera> m_pCamera;
  std::unique_ptr<LightManager> m_pLightManager;
  std::unique_ptr<ModelManager> m_pModelManager;
  std::unique_ptr<Renderer> m_pRenderer;

  GLFWwindow* m_pWindow;

public:
  Application(Application const&) = delete;
  void operator=(Application const&) = delete;
};

extern Application* g_appHandle;