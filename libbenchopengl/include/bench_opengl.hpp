/*************************/
/* (C) Karl Hansson 2017 */
/*************************/
#pragma once

#ifndef BENCH_OPENGL_HPP
#define BENCH_OPENGL_HPP

#include <bench_template.hpp>

class BenchOpenGL : BenchTemplate {
 public:
  BenchOpenGL();
  virtual ~BenchOpenGL();

  virtual void initialize() final override;
  virtual void execute() final override;
  virtual void clean_up() final override;

 private:
};
#endif  // BENCH_OPENGL_HPP
