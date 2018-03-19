#include <iostream>
#include <fstream>
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
void runBenchmark(BENCHMARK_TYPE benchmarkType, int numberOfThreads) {
  std::unique_ptr<BenchTemplate> benchmark = nullptr;

  if (benchmarkType == BENCHMARK_TYPE::OPENGL) {
    benchmark.reset(new BenchOpenGL(1, BENCHMARK_N, BENCHMARK_M));
  } else if (benchmarkType == BENCHMARK_TYPE::VULKAN) {
    benchmark.reset(new BenchVulkan(numberOfThreads, BENCHMARK_N, BENCHMARK_M));
  }

  std::string api = benchmarkType == BENCHMARK_TYPE::OPENGL ? "ogl." : "vk.";
  // std::string cf  = std::string(".") + std::to_string(numberOfThreads) + "."
  // +
  //                  std::to_string(BENCHMARK_N) + "." +
  //                  std::to_string(BENCHMARK_M);
  std::string cf = std::string(".") + std::to_string(numberOfThreads);

  char sorter = 'a';
  auto s      = [&]() { return std::string(1, sorter++) + "."; };

  ResultCollection initialize(api + s() + "initialize" + cf);
  ResultCollection createTrianglesHost(api + s() + "createTrianglesHost" + cf);
  ResultCollection createTrianglesSlow(api + s() + "createTrianglesSlow" + cf);
  ResultCollection createTrianglesSmart(api + s() + "createTrianglesSmart" +
                                        cf);
  ResultCollection createTrianglesFast(api + s() + "createTrianglesFast" + cf);
  ResultCollection createShaderModules(api + s() + "createShaderModules" + cf);
  ResultCollection createPipelines(api + s() + "createPipelines" + cf);

  ResultCollection firstDrawTriangles(api + s() + "firstDrawTriangles" + cf);
  ResultCollection secondDrawTriangles(api + s() + "secondDrawTriangles" + cf);
  ResultCollection thirdDrawTriangles(api + s() + "thirdDrawTriangles" + cf);

  ResultCollection firstDrawPipelines(api + s() + "firstDrawPipelines" + cf);
  ResultCollection secondDrawPipelines(api + s() + "secondDrawPipelines" + cf);
  ResultCollection thirdDrawPipelines(api + s() + "thirdDrawPipelines" + cf);

  ResultCollection optimalMultipleTriangleDraw(
      api + s() + "optimalMultipleTriangleDraw" + cf);
  ResultCollection badMultipleTriangleDraw(api + s() +
                                           "badMultipleTriangleDraw" + cf);

  ResultCollection clean_up(api + s() + "clean_up" + cf);

  ResultCollection* collections[] = {
      &initialize,
      &createTrianglesHost,
      &createTrianglesSlow,
      &createTrianglesSmart,
      &createTrianglesFast,
      &createShaderModules,
      &createPipelines,
      &firstDrawTriangles,
      &secondDrawTriangles,
      &thirdDrawTriangles,
      &firstDrawPipelines,
      &secondDrawPipelines,
      &thirdDrawPipelines,
      &optimalMultipleTriangleDraw,
      &badMultipleTriangleDraw,
      &clean_up,
  };

  for (int i = 0; i < BENCHMARK_OUTER_RUNS; i++) {
    std::cout << "Iteration (" << i + 1 << "/" << BENCHMARK_OUTER_RUNS << ")"
              << std::endl;
    benchmark->startupThreads();

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

    TEST2(singlePipelineDraw, firstDrawTriangles, true);
    TEST2(singlePipelineDraw, secondDrawTriangles, true);
    TEST2(singlePipelineDraw, thirdDrawTriangles, true);

    TEST2(singleTriangleDraw, firstDrawPipelines, true);
    TEST2(singleTriangleDraw, secondDrawPipelines, true);
    TEST2(singleTriangleDraw, thirdDrawPipelines, true);

    TEST(optimalMultipleTriangleDraw, true);
    TEST(badMultipleTriangleDraw, true);

    TEST(clean_up);
  }

  for (auto& collection : collections) {
    std::ofstream output;
    output.open("./output/" + collection->getFilename() + ".txt");
    output << *collection << std::endl;
  }

  // std::cout << initialize << std::endl;
  // std::cout << createTrianglesHost << std::endl;
  // std::cout << createTrianglesSlow << std::endl;
  // std::cout << createTrianglesSmart << std::endl;
  // std::cout << createTrianglesFast << std::endl;
  // std::cout << createShaderModules << std::endl;
  // std::cout << createPipelines << std::endl;
  // std::cout << firstDraw << std::endl;
  // std::cout << secondDraw << std::endl;
  // std::cout << thirdDraw << std::endl;
  // std::cout << clean_up << std::endl;
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

  runBenchmark(BENCHMARK_TYPE::VULKAN, 1);
  runBenchmark(BENCHMARK_TYPE::VULKAN, 2);
  runBenchmark(BENCHMARK_TYPE::VULKAN, 4);
  runBenchmark(BENCHMARK_TYPE::VULKAN, 8);
  runBenchmark(BENCHMARK_TYPE::OPENGL, 1);

  // { BenchVulkan bv(1, 10, 10); }
  // { BenchOpenGL bo(1, 10, 10); }

  std::cin.ignore();
  std::cin.get();
  return 0;
}