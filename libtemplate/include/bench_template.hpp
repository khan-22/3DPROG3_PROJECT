#pragma once

#ifndef BENCH_TEMPLATE_HPP
#define BENCH_TEMPLATE_HPP

#include <string>

class BenchTemplate {
 public:
  BenchTemplate(int numberOfThreads, int N, int M);
  virtual ~BenchTemplate();

  virtual void initialize() = 0;

  virtual void createShaderModules() = 0;
  virtual void createPipelines() = 0;

  virtual void firstDraw() = 0;
  virtual void secondDraw() = 0;
  virtual void thirdDraw() = 0;

  virtual void clean_up() = 0;

 protected:
  const int _WIDTH = 800;
  const int _HEIGHT = 600;
  const std::string _TITLE = "Benchmark";

  int _numberOfThreads;
  int _N;
  int _M;

 private:
};

#endif  // BENCH_TEMPLATE_HPP