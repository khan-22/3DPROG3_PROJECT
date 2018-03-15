#pragma once

#ifndef BENCH_OPENGL_HPP
#define BENCH_OPENGL_HPP

#include <bench_template.hpp>
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
#include <GLFW/glfw3.h>
#include <array>
#include <utility>

#include <string>


class BenchOpenGL : public BenchTemplate {
 public:
  BenchOpenGL(int numberOfThreads, int N, int M);
  virtual ~BenchOpenGL();

  virtual void initialize(ResultCollection& resultCollection) final override;

  virtual void createShaderModules(
      ResultCollection& resultCollection) final override;
  virtual void createPipelines(
      ResultCollection& resultCollection) final override;

  virtual void firstDraw(ResultCollection& resultCollection) final override;
  virtual void secondDraw(ResultCollection& resultCollection) final override;
  virtual void thirdDraw(ResultCollection& resultCollection) final override;

  virtual void clean_up(ResultCollection& resultCollection) final override;

 private:  // Functions
 private:  // Varibles
  GLFWwindow* _window = nullptr;
  std::array<std::pair<std::string, std::string>, BENCHMARK_N> shaderPairArr;
};
#endif  // BENCH_OPENGL_HPP
