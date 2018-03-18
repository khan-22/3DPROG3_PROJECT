#pragma once

#ifndef BENCH_OPENGL_HPP
#define BENCH_OPENGL_HPP

#include <bench_template.hpp>
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <array>
#include <utility>

#include <string>

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
  virtual void singlePipelineDraw(ResultCollection& resultCollection,
                                  bool              device) final override;

  virtual void optimalMultipleTriangleDraw(ResultCollection& resultCollection,
                                           bool device) final override;
  virtual void badMultipleTriangleDraw(ResultCollection& resultCollection,
                                       bool              device) final override;

  virtual void clean_up(ResultCollection& resultCollection) final override;

 private:  // Functions
 private:  // Varibles
  GLFWwindow* _window = nullptr;
  std::array<std::pair<std::string, std::string>, BENCHMARK_N>
                                      shaderPairArr;  // Array containing all shader source files
  std::array<gl::GLuint, BENCHMARK_N> shaderProgramArr;

  std::array<gl::GLuint, BENCHMARK_N> VBOArr;  // vertex buffer object
  std::array<gl::GLuint, BENCHMARK_N> VAOArr;  // vertex array object
};
#endif  // BENCH_OPENGL_HPP
