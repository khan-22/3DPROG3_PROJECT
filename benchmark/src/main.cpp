#include <iostream>

#include <bench_template.hpp>
#include <bench_vulkan.hpp>

int main()
{
  std::cout << "Hello World" << std::endl;

  {
    BenchVulkan bv;
  }

  std::cin.ignore();
  std::cin.get();
  return 0;
}