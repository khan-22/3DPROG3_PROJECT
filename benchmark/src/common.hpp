#pragma once

#ifndef COMMON_HPP
#define COMMON_HPP

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std::chrono;
using Time = std::chrono::high_resolution_clock::time_point;
using TimeSpan = std::chrono::duration<double>;

#define BENCHMARK_OUTER_RUNS 100
#define BENCHMARK_N 100
#define BENCHMARK_M 100

#endif  // COMMON_HPP
