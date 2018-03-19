#pragma once

#ifndef BENCH_TEMPLATE_HPP
#define BENCH_TEMPLATE_HPP

#include <string>
#include <utility>
#include <array>
#include <mutex>
#include "result_collection.hpp"

struct Vertex {
  float x, y, z;
};

class BenchTemplate {
 public:
  BenchTemplate(int numberOfThreads, int N, int M);
  virtual ~BenchTemplate();

  virtual void startupThreads(){};

  virtual void initialize(ResultCollection& resultCollection) = 0;

  virtual void createTrianglesHost(ResultCollection& resultCollection)  = 0;
  virtual void createTrianglesSlow(ResultCollection& resultCollection)  = 0;
  virtual void createTrianglesSmart(ResultCollection& resultCollection) = 0;
  virtual void createTrianglesFast(ResultCollection& resultCollection)  = 0;
  virtual void intermediateTriangleCleanUp()                            = 0;

  virtual void createShaderModules(ResultCollection& resultCollection) = 0;
  virtual void createPipelines(ResultCollection& resultCollection)     = 0;

  virtual void singleTriangleDraw(ResultCollection& resultCollection,
                                  bool              device)          = 0;
  virtual void singlePipelineDraw(ResultCollection& resultCollection,
                                  bool              device)          = 0;
  virtual void optimalMultipleTriangleDraw(ResultCollection& resultCollection,
                                           bool              device) = 0;
  virtual void badMultipleTriangleDraw(ResultCollection& resultCollection,
                                       bool              device)     = 0;

  virtual void clean_up(ResultCollection& resultCollection) = 0;

 protected:  // Functions
  std::pair<std::string, std::string> loadShaderSource();
  std::string                         getNextDefine();
  virtual std::array<Vertex, 3>       getNextTriangle();

 protected:  // Variables
  const uint32_t    _WIDTH  = 800;
  const uint32_t    _HEIGHT = 600;
  const std::string _TITLE  = "Benchmark";

  int _numberOfThreads;
  int _N;
  int _M;

 private:
  // std::mutex _triangleMutex;
};

#endif  // BENCH_TEMPLATE_HPP