#include "result_collection.hpp"

//
// Helper class
void Timer::start(std::string columnHead) {
  _columnhead = std::move(columnHead);
  _start      = high_resolution_clock::now();
}
void Timer::stop() {
  _end = high_resolution_clock::now();
  time = duration_cast<TimeSpan>(_end - _start).count();
}

const std::string& Timer::getColumnHead() const {
  return _columnhead;
}

//
// Place to store all our results
ResultCollection::ResultCollection() {
  _results.reserve(BENCHMARK_OUTER_RUNS);
  _results.emplace_back();  //
  _results.back().reserve(20);
}

ResultCollection::~ResultCollection() {
}

void ResultCollection::addResult(Timer t) {
  _results.back().push_back(t);  //
}

void ResultCollection::averageWith(ResultCollection& resultCollection) {
  if (_results.back().size() == 0 &&
      resultCollection._results.back().size() != 0) {
    // If this is empty, then just adopt the results of the other
    _results = resultCollection._results;
    return;
  } else if (_results.back().size() !=
             resultCollection._results.back().size()) {
    // Otherwise, if there's a mismatch then we have an error!
    std::cerr << "Mismatch in result collection sizes!" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  int    totalWeight = _averageWeight + resultCollection._averageWeight;
  double thisRatio   = static_cast<double>(_averageWeight) / totalWeight;
  double otherRatio =
      static_cast<double>(resultCollection._averageWeight) / totalWeight;

  for (int i = 0; i < _results.back().size(); i++) {
    _results[i].back().time =
        thisRatio * _results[i].back().time +
        otherRatio * resultCollection._results[i].back().time;
  }

  // Increase weight for future averaging
  _averageWeight++;
}

void ResultCollection::nextIteration() {
  _results.emplace_back();
  _results.back().reserve(20);
  _averageWeight = 1;
}

//
// stream overload to easily print our results
std::ostream& operator<<(std::ostream& a, const ResultCollection& b) {
  a << "Timestamp\t";
  for (auto& result : b._results[0]) {
    a << result.getColumnHead() << "\t";
  }
  for (auto& row : b._results) {
    if (row.size() > 0) {
      a << std::endl;
      Time timestamp = row[0]._start;
      a << timestamp.time_since_epoch().count() << "\t";
      for (auto& result : row) {
        a << result.time << "\t";
      }
    }
  }

  a << std::endl;
  return a;
}
