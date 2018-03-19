#pragma once

#ifndef COMMON_HPP
#define COMMON_HPP

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std::chrono;
using Time     = std::chrono::high_resolution_clock::time_point;
using TimeSpan = std::chrono::duration<double>;

#define BENCHMARK_OUTER_RUNS 5
#define BENCHMARK_N 400
#define BENCHMARK_M 400

static_assert(BENCHMARK_N >= BENCHMARK_M,
              "BENCHMARK_N must be GREATER or EQUAL to BENCHMARK_M");

#endif  // COMMON_HPP
