#pragma once

#ifndef BENCH_TEMPLATE_HPP
#define BENCH_TEMPLATE_HPP

#include <string>
#include <utility>

#include "result_collection.hpp"

class BenchTemplate {
 public:
  BenchTemplate(int numberOfThreads, int N, int M);
  virtual ~BenchTemplate();

  virtual void initialize(ResultCollection& resultCollection) = 0;

  virtual void createShaderModules(ResultCollection& resultCollection) = 0;
  virtual void createPipelines(ResultCollection& resultCollection)     = 0;

  virtual void firstDraw(ResultCollection& resultCollection)  = 0;
  virtual void secondDraw(ResultCollection& resultCollection) = 0;
  virtual void thirdDraw(ResultCollection& resultCollection)  = 0;

  virtual void clean_up(ResultCollection& resultCollection) = 0;

 protected:  // Functions
  std::pair<std::string, std::string> loadShaderSource();

 protected:  // Variables
  const uint32_t    _WIDTH  = 800;
  const uint32_t    _HEIGHT = 600;
  const std::string _TITLE  = "Benchmark";

  int _numberOfThreads;
  int _N;
  int _M;

 private:
};

#endif  // BENCH_TEMPLATE_HPP