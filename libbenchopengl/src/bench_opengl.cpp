#include "bench_opengl.hpp"

#include <iostream>

#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>

#include <GLFW/glfw3.h>

BenchOpenGL::BenchOpenGL(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  //
  GLFWwindow* window = nullptr;

  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowShouldClose(window, true);

  glbinding::Binding::initialize();

  // while (!glfwWindowShouldClose(window)) {
  //   glfwSwapBuffers(window);
  //   glfwPollEvents();
  // }

  std::cout << "Created BenchOpenGL" << std::endl;
}

BenchOpenGL::~BenchOpenGL() {
  //
  std::cout << "Destroyed BenchOpenGL" << std::endl;
  glfwTerminate();
}

void BenchOpenGL::initialize() {}

void BenchOpenGL::createShaderModules() {}
void BenchOpenGL::createPipelines() {}

void BenchOpenGL::firstDraw() {}
void BenchOpenGL::secondDraw() {}
void BenchOpenGL::thirdDraw() {}

void BenchOpenGL::clean_up() {}
