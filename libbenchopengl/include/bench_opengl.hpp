#pragma once

#ifndef BENCH_OPENGL_HPP
#define BENCH_OPENGL_HPP

#include <bench_template.hpp>

#include <GLFW/glfw3.h>

class BenchOpenGL : public BenchTemplate {
 public:
  BenchOpenGL(int numberOfThreads, int N, int M);
  virtual ~BenchOpenGL();

  virtual void initialize(ResultCollection& resultCollection) final override;

  virtual void createTriangles(
      ResultCollection& resultCollection) final override;

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
};
#endif  // BENCH_OPENGL_HPP
