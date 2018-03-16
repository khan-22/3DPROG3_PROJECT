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

  virtual void createTrianglesHost(
      ResultCollection& resultCollection) final override;
  virtual void createTrianglesSlow(
      ResultCollection& resultCollection) final override;
  virtual void createTrianglesSmart(
      ResultCollection& resultCollection) final override;
  virtual void createTrianglesFast(
      ResultCollection& resultCollection) final override;
  virtual void intermediateTriangleCleanUp() final override;

  virtual void createShaderModules(
      ResultCollection& resultCollection) final override;
  virtual void createPipelines(
      ResultCollection& resultCollection) final override;

  virtual void singleTriangleDraw(ResultCollection& resultCollection,
                                  bool              device) final override;
  virtual void optimalMultipleTriangleDraw(ResultCollection& resultCollection,
                                           bool device) final override;
  virtual void badMultipleTriangleDraw(ResultCollection& resultCollection,
                                       bool              device) final override;

  virtual void clean_up(ResultCollection& resultCollection) final override;

 private:  // Functions
 private:  // Varibles
  GLFWwindow* _window = nullptr;
};
#endif  // BENCH_OPENGL_HPP
