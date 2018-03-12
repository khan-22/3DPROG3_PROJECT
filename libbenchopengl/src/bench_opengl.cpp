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

void BenchOpenGL::initialize(ResultCollection& resultCollection) {
  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  Timer t;
  t.start("Window");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_WIDTH, _HEIGHT, _TITLE.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(_window);
  t.stop();
  resultCollection.addResult(t);

  glbinding::Binding::initialize();
}

void BenchOpenGL::createShaderModules(ResultCollection& resultCollection) {
  auto sourcePair = loadShaderSource();
}

void BenchOpenGL::createPipelines(ResultCollection& resultCollection) {
}

void BenchOpenGL::firstDraw(ResultCollection& resultCollection) {
}

void BenchOpenGL::secondDraw(ResultCollection& resultCollection) {
}

void BenchOpenGL::thirdDraw(ResultCollection& resultCollection) {
}

void BenchOpenGL::clean_up(ResultCollection& resultCollection) {
  glfwDestroyWindow(_window);
  glfwTerminate();  //
}
