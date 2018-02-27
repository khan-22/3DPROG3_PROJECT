#include "bench_opengl.hpp"

#include <iostream>

#include <glbinding/gl/gl.h>
#include <glbinding/Binding.h>

BenchOpenGL::BenchOpenGL() : BenchTemplate() {
  //
  std::cout << "Created BenchOpenGL" << std::endl;

	glbinding::Binding::initialize();
}

BenchOpenGL::~BenchOpenGL() {
  //
  std::cout << "Destroyed BenchOpenGL" << std::endl;
}

void BenchOpenGL::initialize() {}

void BenchOpenGL::execute() {}

void BenchOpenGL::clean_up() {}