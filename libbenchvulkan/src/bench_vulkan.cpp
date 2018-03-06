#include "bench_vulkan.hpp"

#include <iostream>

#include <vulkan/vulkan.hpp>

BenchVulkan::BenchVulkan(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  //
  std::cout << "Created BenchVulkan" << std::endl;
}

BenchVulkan::~BenchVulkan() {
  std::cout << "Destroyed BenchVulkan" << std::endl;
}

void BenchVulkan::initialize() {}

void BenchVulkan::createShaderModules() {}
void BenchVulkan::createPipelines() {}

void BenchVulkan::firstDraw() {}
void BenchVulkan::secondDraw() {}
void BenchVulkan::thirdDraw() {}

void BenchVulkan::clean_up() {}
