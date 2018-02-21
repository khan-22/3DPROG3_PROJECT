#include "bench_dx11dc.hpp"

#include <iostream>

BenchDX11DC::BenchDX11DC() : BenchTemplate() {
  //
  std::cout << "Created BenchDX11DC" << std::endl;
}

BenchDX11DC::~BenchDX11DC() {
  std::cout << "Destroyed BenchDX11DC" << std::endl;
}

void BenchDX11DC::initialize() {}

void BenchDX11DC::execute() {}

void BenchDX11DC::clean_up() {}
