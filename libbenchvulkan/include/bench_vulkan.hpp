#pragma once

#ifndef BENCH_VULKAN_HPP
#define BENCH_VULKAN_HPP

#include <bench_template.hpp>

class BenchVulkan : public BenchTemplate {
public:
  BenchVulkan();
  virtual ~BenchVulkan();

  virtual void initialize() final override;
  virtual void execute() final override;
  virtual void clean_up() final override;

private:

};



#endif //BENCH_VULKAN_HPP