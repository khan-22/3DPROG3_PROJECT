#pragma once

#ifndef BENCH_TEMPLATE_HPP
#define BENCH_TEMPLATE_HPP

class BenchTemplate {
public:
  BenchTemplate();
  virtual ~BenchTemplate();

  virtual void initialize() = 0;
  virtual void execute() = 0;
  virtual void clean_up() = 0;

private:

};


#endif //BENCH_TEMPLATE_HPP