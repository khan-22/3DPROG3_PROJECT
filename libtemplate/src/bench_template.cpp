#include "bench_template.hpp"

#include <iostream>
#include <fstream>

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
  }

  std::ifstream f_input("./res/fragment.glsl");
  std::string   f_shaderText;
  if (f_input.is_open()) {
    f_shaderText = std::string((std::istreambuf_iterator<char>(f_input)),
                               std::istreambuf_iterator<char>());
    f_input.close();
  }

  return {v_shaderText, f_shaderText};
}
