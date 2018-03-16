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

#define TEST(name, ...)                     \
  {                                         \
    Timer timer;                            \
    timer.start("Total");                   \
    benchmark->name((name), ##__VA_ARGS__); \
    timer.stop();                           \
                                            \
    (name).addResult(timer);                \
    (name).nextIteration();                 \
  }

#define TEST2(name, collection, ...)              \
  {                                               \
    Timer timer;                                  \
    timer.start("Total");                         \
    benchmark->name((collection), ##__VA_ARGS__); \
    timer.stop();                                 \
                                                  \
    (collection).addResult(timer);                \
    (collection).nextIteration();                 \
  }
//
// The test routine
void runBenchmark(BENCHMARK_TYPE benchmarkType) {
  std::unique_ptr<BenchTemplate> benchmark = nullptr;

  if (benchmarkType == BENCHMARK_TYPE::OPENGL) {
    benchmark.reset(new BenchOpenGL(1, 10, 10));
  } else if (benchmarkType == BENCHMARK_TYPE::VULKAN) {
    benchmark.reset(new BenchVulkan(2, 10, 10));
  }

  ResultCollection initialize;
  ResultCollection createTrianglesHost;
  ResultCollection createTrianglesSlow;
  ResultCollection createTrianglesSmart;
  ResultCollection createTrianglesFast;
  ResultCollection createShaderModules;
  ResultCollection createPipelines;

  ResultCollection firstDraw;
  ResultCollection secondDraw;
  ResultCollection thirdDraw;

  ResultCollection clean_up;

  for (int i = 0; i < BENCHMARK_OUTER_RUNS; i++) {
    TEST(initialize);
    TEST(createTrianglesHost);
    benchmark->intermediateTriangleCleanUp();
    TEST(createTrianglesSlow);
    benchmark->intermediateTriangleCleanUp();
    TEST(createTrianglesSmart);
    benchmark->intermediateTriangleCleanUp();
    TEST(createTrianglesFast);
    TEST(createShaderModules);
    TEST(createPipelines);

    TEST2(singleTriangleDraw, firstDraw, true);
    TEST2(singleTriangleDraw, secondDraw, true);
    TEST2(singleTriangleDraw, thirdDraw, true);

    TEST(clean_up);
  }

  std::cout << initialize << std::endl;
  std::cout << createTrianglesHost << std::endl;
  std::cout << createTrianglesSlow << std::endl;
  std::cout << createTrianglesSmart << std::endl;
  std::cout << createTrianglesFast << std::endl;
  std::cout << createShaderModules << std::endl;
  std::cout << createPipelines << std::endl;
  std::cout << firstDraw << std::endl;
  std::cout << secondDraw << std::endl;
  std::cout << thirdDraw << std::endl;
  std::cout << clean_up << std::endl;
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