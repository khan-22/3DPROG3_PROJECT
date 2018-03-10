#include "result_collection.hpp"

//
// Helper class
void Timer::start() { _start = high_resolution_clock::now(); }
void Timer::stop() {
  _end = high_resolution_clock::now();
  time = duration_cast<TimeSpan>(_end - _start).count();
}

//
// Place to store all our results
ResultCollection::ResultCollection() {
  _results.reserve(BENCHMARK_N);  //
}

ResultCollection::~ResultCollection() {}

void ResultCollection::addResult(Timer t) {
  _results.push_back(t);  //
}

void ResultCollection::averageWith(ResultCollection& resultCollection) {
  if (_results.size() == 0 && resultCollection._results.size() != 0) {
    // If this is empty, then just adopt the results of the other
    _results = resultCollection._results;
    return;
  } else if (_results.size() != resultCollection._results.size()) {
    // Otherwise, if there's a mismatch then we have an error!
    std::cerr << "Mismatch in result collection sizes!" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  int totalWeight = _averageWeight + resultCollection._averageWeight;
  double thisRatio = static_cast<double>(_averageWeight) / totalWeight;
  double otherRatio =
      static_cast<double>(resultCollection._averageWeight) / totalWeight;

  for (int i = 0; i < _results.size(); i++) {
    _results[i].time = thisRatio * _results[i].time +
                       otherRatio * resultCollection._results[i].time;
  }

  // Increase weight for future averaging
  _averageWeight++;
}

//
// stream overload to easily print our results
std::ostream& operator<<(std::ostream& a, const ResultCollection& b) {
  for (auto& result : b._results) {
    a << result.time << std::endl;
  }
  a << std::endl;
  return a;
}
