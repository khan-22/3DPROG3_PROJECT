#pragma once

#ifndef RESULT_COLLECTION_HPP
#define RESULT_COLLECTION_HPP

#include <iostream>
#include <string>
#include <vector>

#include "common.hpp"

struct ResultCollection;

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

  friend std::ostream& operator<<(std::ostream& a, const ResultCollection& b);
};

//
// Place to store all our results
struct ResultCollection {
  ResultCollection(std::string filename);
  ~ResultCollection();

  void addResult(Timer t);

  void averageWith(ResultCollection& resultCollection);
  void nextIteration();

  std::string getFilename() const;

 private:
  std::vector<std::vector<Timer>> _results;
  std::string                     _filename;

  int _averageWeight = 1;

  friend std::ostream& operator<<(std::ostream& a, const ResultCollection& b);
};

#endif  // RESULT_COLLECTION_HPP
