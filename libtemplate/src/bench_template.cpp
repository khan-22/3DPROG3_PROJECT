#include "bench_template.hpp"

#include <iostream>

BenchTemplate::BenchTemplate(int numberOfThreads, int N, int M)
    : _numberOfThreads(numberOfThreads), _N(N), _M(M) {
  //
  std::cout << "Created template" << std::endl;
}

BenchTemplate::~BenchTemplate() {
  //
  std::cout << "Destroyed template" << std::endl;
}
