#include <iostream>
#include <string>

#include <bench_template.hpp>

#include <bench_opengl.hpp>
#include <bench_vulkan.hpp>

int main() {
#if defined(NDEBUG)
	std::string build_config = "(RELEASE)";
#else
	std::string build_config = "(DEBUG)";
#endif

  std::cout << "Running Benchmarks... " << build_config << std::endl;


  { BenchVulkan bv; }
  { BenchOpenGL bo; }

  std::cin.ignore();
  std::cin.get();
  return 0;
}