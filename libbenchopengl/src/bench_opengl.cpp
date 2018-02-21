#include "bench_opengl.hpp"

#include <iostream>

BenchOpenGL::BenchOpenGL() : BenchTemplate() {
  //
  std::cout << "Created BenchOpenGL" << std::endl;
}

BenchOpenGL::~BenchOpenGL() {
  //
  std::cout << "Destroyed BenchOpenGL" << std::endl;
}

void BenchOpenGL::initialize() {}

void BenchOpenGL::execute() {}

void BenchOpenGL::clean_up() {}