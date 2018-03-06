#pragma once

#ifndef BENCH_OPENGL_HPP
#define BENCH_OPENGL_HPP

#include <bench_template.hpp>

#include <GLFW/glfw3.h>

class BenchOpenGL : public BenchTemplate {
 public:
  BenchOpenGL(int numberOfThreads, int N, int M);
  virtual ~BenchOpenGL();

  virtual void initialize() final override;

  virtual void createShaderModules() final override;
  virtual void createPipelines() final override;

  virtual void firstDraw() final override;
  virtual void secondDraw() final override;
  virtual void thirdDraw() final override;

  virtual void clean_up() final override;

 private:  // Functions
 private:  // Varibles
  GLFWwindow* _window = nullptr;
};
#endif  // BENCH_OPENGL_HPP
