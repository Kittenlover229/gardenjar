#include "WindowManager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace gardenjar::windowing;

WindowManager::WindowManager(core::Workspace &ws) : ws(ws) {
  glfwSetErrorCallback([](int errc, const char *str) {
    fprintf(stderr, "Glfw Error %d: %s\n", errc, str);
    abort();
  });

  if (!glfwInit()) abort();

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  glfw_window = glfwCreateWindow(1280, 720, "Garden Jar", NULL, NULL);
  if (glfw_window == NULL) abort();

  glfwSetWindowUserPointer(glfw_window, (void *)this);
  glfwMakeContextCurrent(glfw_window);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGuiStyle &style = ImGui::GetStyle();

  ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

WindowManager::~WindowManager() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
}

void WindowManager::join() {
  glfwShowWindow(glfw_window);

  while (!glfwWindowShouldClose(glfw_window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto &win : windows)
      if (win->keep_alive()) win->draw(*this);

    ImGuiIO &io = ImGui::GetIO();
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glfw_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(glfw_window);
  }
}

void WindowManager::add_window(std::shared_ptr<Window> &&win) {
  // Cleanup space for new windows
  std::remove_if(windows.begin(), windows.end(),
                 [](auto win) { return !win->keep_alive(); });

  windows.push_back(win);
}
