#pragma once

#ifndef BENCH_DX11DC_HPP
#define BENCH_DX11DC_HPP

#include <bench_template.hpp>

class BenchDX11DC : public BenchTemplate {
 public:
  BenchDX11DC();
  virtual ~BenchDX11DC();

  virtual void initialize() final override;
  virtual void execute() final override;
  virtual void clean_up() final override;

 private:
};
#endif  // BENCH_DX11DC_HPP
