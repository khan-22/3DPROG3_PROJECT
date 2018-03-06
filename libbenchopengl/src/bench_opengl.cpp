#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>

#include "bench_opengl.hpp"

#include <iostream>

#include <GLFW/glfw3.h>
//
BenchOpenGL::BenchOpenGL(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  std::cout << "Started OpenGL Benchmark... (" << numberOfThreads << ", " << N
            << ", " << M << ")" << std::endl;
}

BenchOpenGL::~BenchOpenGL() {
  //
  std::cout << "Finished OpenGL Benchmark" << std::endl;
}

void BenchOpenGL::initialize() {
  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_WIDTH, _HEIGHT, _TITLE.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(_window);
  glbinding::Binding::initialize();
}

void BenchOpenGL::createShaderModules() {
}

void BenchOpenGL::createPipelines() {
}

void BenchOpenGL::firstDraw() {
}

void BenchOpenGL::secondDraw() {
}

void BenchOpenGL::thirdDraw() {
}

void BenchOpenGL::clean_up() {
  glfwDestroyWindow(_window);
  glfwTerminate();  //
}
