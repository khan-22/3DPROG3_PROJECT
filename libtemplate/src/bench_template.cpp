#include "bench_template.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>

BenchTemplate::BenchTemplate(int numberOfThreads, int N, int M)
    : _numberOfThreads(numberOfThreads), _N(N), _M(M) {
  //
  // std::cout << "Created template" << std::endl;
}

BenchTemplate::~BenchTemplate() {
  //
  // std::cout << "Destroyed template" << std::endl;
}

std::pair<std::string, std::string> BenchTemplate::loadShaderSource() {
  std::ifstream v_input("./res/vertex.glsl");
  std::string   v_shaderText;
  if (v_input.is_open()) {
    v_shaderText = std::string((std::istreambuf_iterator<char>(v_input)),
                               std::istreambuf_iterator<char>());
    v_input.close();
  } else {
    std::cerr << "Failed to find vertex shader file!" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  std::ifstream f_input("./res/fragment.glsl");
  std::string   f_shaderText;
  if (f_input.is_open()) {
    f_shaderText = std::string((std::istreambuf_iterator<char>(f_input)),
                               std::istreambuf_iterator<char>());
    f_input.close();
  } else {
    std::cerr << "Failed to find fragment shader file!" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  return {v_shaderText, f_shaderText};
}

std::string BenchTemplate::getNextDefine() {
  double r = static_cast<double>(rand() % 255) / 255;
  double g = static_cast<double>(rand() % 255) / 255;
  double b = static_cast<double>(rand() % 255) / 255;

  return {"#version 450\n\n#define COLOR1 " + std::to_string(r) + "," +
          std::to_string(g) + "," + std::to_string(b) + "\n\n"};
}

std::array<Vertex, 3> BenchTemplate::getNextTriangle() {
  return {Vertex{(rand() % 512) / 256.f - 1.f,
                 (rand() % 512) / 256.f - 1.f,
                 (rand() % 512) / 256.f - 1.f},
          Vertex{(rand() % 512) / 256.f - 1.f,
                 (rand() % 512) / 256.f - 1.f,
                 (rand() % 512) / 256.f - 1.f},
          Vertex{(rand() % 512) / 256.f - 1.f,
                 (rand() % 512) / 256.f - 1.f,
                 (rand() % 512) / 256.f - 1.f}};
}
