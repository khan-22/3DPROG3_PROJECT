#include <iostream>

#include <bench_template.hpp>

#include <bench_dx11dc.hpp>
#include <bench_opengl.hpp>
#include <bench_vulkan.hpp>

int main() {
  std::cout << "Hello World" << std::endl;

  { BenchVulkan bv; }
  { BenchOpenGL bo; }
  { BenchDX11DC bd; }

  std::cin.ignore();
  std::cin.get();
  return 0;
}