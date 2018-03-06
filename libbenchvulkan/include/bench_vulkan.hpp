#pragma once

#ifndef BENCH_VULKAN_HPP
#define BENCH_VULKAN_HPP

#include <bench_template.hpp>

class BenchVulkan : public BenchTemplate {
 public:
  BenchVulkan(int numberOfThreads, int N, int M);
  virtual ~BenchVulkan();

  virtual void initialize() final override;

  virtual void createShaderModules() final override;
  virtual void createPipelines() final override;

  virtual void firstDraw() final override;
  virtual void secondDraw() final override;
  virtual void thirdDraw() final override;

  virtual void clean_up() final override;

 private:
};

#endif  // BENCH_VULKAN_HPP