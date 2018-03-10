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
  void   start(std::string columnHead);
  void   stop();

  const std::string& getColumnHead() const;

 private:
  std::string _columnhead;

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
  void nextIteration();

 private:
  std::vector<std::vector<Timer>> _results;

  int _averageWeight = 1;

  friend std::ostream& operator<<(std::ostream& a, const ResultCollection& b);
};

#endif  // RESULT_COLLECTION_HPP
