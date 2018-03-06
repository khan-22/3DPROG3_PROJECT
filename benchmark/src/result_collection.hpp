#pragma once

#ifndef RESULT_COLLECTION_HPP
#define RESULT_COLLECTION_HPP

#include <iostream>
#include <string>
#include <vector>

#include "common.hpp"

//
// Helper class
struct Timer {
  double time;
  void start();
  void stop();

 private:
  Time _start;
  Time _end;
};

//
// Place to store all our results
struct ResultCollection {
  ResultCollection();
  ~ResultCollection();

  void addResult(Timer t);

  void averageWith(ResultCollection& resultCollection);

 private:
  std::vector<Timer> _results;

  int _averageWeight = 1;

  friend std::ostream& operator<<(std::ostream& a, const ResultCollection& b);
};

#endif  // RESULT_COLLECTION_HPP
