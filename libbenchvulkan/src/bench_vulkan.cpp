#include "bench_vulkan.hpp"

#include <iostream>

BenchVulkan::BenchVulkan() : BenchTemplate() {
  //
  std::cout << "Created BenchVulkan" << std::endl;
}

BenchVulkan::~BenchVulkan() {
  std::cout << "Destroyed BenchVulkan" << std::endl;
}

void BenchVulkan::initialize() {

}

void BenchVulkan::execute() {
  
}

void BenchVulkan::clean_up() {
  
}