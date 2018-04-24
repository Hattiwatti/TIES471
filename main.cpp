#define GLEW_STATIC

#include "Application.h"
#include <thread>
#include <Windows.h>

Application* g_appHandle = nullptr;

static void InitAndRun()
{
  g_appHandle = new Application();
  if (g_appHandle->Initialize())
    g_appHandle->Run();

  delete g_appHandle;
}

int main(int argc, char* argv[])
{
  std::thread th(InitAndRun);
  th.join();

  return 0;
}