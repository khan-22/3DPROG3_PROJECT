#include "bench_vulkan.hpp"

#include <iostream>

#define GET_PROC(name)                                                        \
  [&]() {                                                                     \
    auto ______pf = (PFN_##name)instance.getProcAddr((#name));                \
    if (______pf == nullptr) {                                                \
      fprintf(stderr, "%s (%s)\n", "ERROR: Failed to load procedure", #name); \
      std::exit(EXIT_FAILURE);                                                \
    }                                                                         \
    return ______pf;                                                          \
  }()

#define CRITICAL(call, msg)                                 \
  {                                                         \
    if ((call) != vk::Result::eSuccess) {                   \
      fprintf(stderr, "%s %s\n", "ERROR: Failed to ", msg); \
      std::exit(EXIT_FAILURE);                              \
    }                                                       \
  }

//
BenchVulkan::BenchVulkan(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  //
  std::cout << "Started Vulkan Benchmark... (" << numberOfThreads << ", " << N
            << ", " << M << ")" << std::endl;
}

BenchVulkan::~BenchVulkan() {
  std::cout << "Finished Vulkan Benchmark" << std::endl;
}

void BenchVulkan::initialize() {
  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_WIDTH, _HEIGHT, _TITLE.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(_window);
}

void BenchVulkan::createShaderModules() {
}

void BenchVulkan::createPipelines() {
}

void BenchVulkan::firstDraw() {
}

void BenchVulkan::secondDraw() {
}

void BenchVulkan::thirdDraw() {
}

void BenchVulkan::clean_up() {
  glfwDestroyWindow(_window);
  glfwTerminate();  //
}
