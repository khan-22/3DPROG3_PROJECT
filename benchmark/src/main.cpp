#include <iostream>
#include <string>
#include <vector>

#include <bench_template.hpp>

#include <bench_opengl.hpp>
#include <bench_vulkan.hpp>

#include <chrono>
#include <memory>
#include <ratio>

#include "common.hpp"
#include "result_collection.hpp"

enum class BENCHMARK_TYPE {
  OPENGL,
  VULKAN,
};

//
// The test routine
void runBenchmark(BENCHMARK_TYPE benchmarkType) {
  std::unique_ptr<BenchTemplate> benchmark = nullptr;

  if (benchmarkType == BENCHMARK_TYPE::OPENGL) {
    benchmark.reset(new BenchOpenGL(1, 10, 10));
  } else if (benchmarkType == BENCHMARK_TYPE::VULKAN) {
    benchmark.reset(new BenchVulkan(1, 10, 10));
  }

  ResultCollection initialization;
  ResultCollection shaderModules;
  ResultCollection pipelines;
  ResultCollection firstDraw;
  ResultCollection secondDraw;
  ResultCollection thirdDraw;
  ResultCollection clean_up;
  for (int i = 0; i < BENCHMARK_OUTER_RUNS; i++) {
    // Initialization
    {
      ResultCollection results;

      Timer timer;
      timer.start();
      benchmark->initialize();
      timer.stop();

      results.addResult(timer);

      initialization.averageWith(results);
    }

    // shaderModules
    {
      ResultCollection results;

      for (int j = 0; j < BENCHMARK_N; j++) {
        Timer timer;
        timer.start();
        benchmark->createShaderModules();
        timer.stop();
        results.addResult(timer);
      }

      shaderModules.averageWith(results);
    }

    benchmark->clean_up();
  }

  // std::cout << initialization << std::endl;
  // std::cout << shaderModules << std::endl;
}

//
//
int main() {
#if defined(NDEBUG)
  std::string build_config = "(RELEASE)";
#else
  std::string build_config = "(DEBUG)";
#endif

  std::cout << "Running Benchmarks... " << build_config << std::endl;

  // Timer t;
  // t.start();
  // for (int i = 0; i < 1000000; i++) {
  //   std::cout << "A" << std::endl;
  // }
  // t.stop();
  // ResultCollection r;
  // r.addResult(t);
  //
  // std::cout << r << std::endl;

  runBenchmark(BENCHMARK_TYPE::OPENGL);
  runBenchmark(BENCHMARK_TYPE::VULKAN);

  // { BenchVulkan bv(1, 10, 10); }
  // { BenchOpenGL bo(1, 10, 10); }

  std::cin.ignore();
  std::cin.get();
  return 0;
}