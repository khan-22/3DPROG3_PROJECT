# Driver Overhead Benchmark [3DPROG3 PROJECT]
Our project in DV2551

Members: Karl Hansson, Filip Pentikäinen, Peyman Torabi

The aim of our project is to measure the driver overhead in different graphics 
APIs (Vulkan, together with most probably both DX11 and OpenGL) to determine
how significant the difference may be during different stages of the program
(initialization, transfer, execution, clean up, etc...).

We aim to do this by essentially implementing the same program multiple times as
static libraries, one for each API, which can be executed from a single benchmark
executable that collects our desired metrics. Our main metric is time, but the
possibility to measure memory overhead also exists.

Since we want to measure driver overhead we want to try to have as much interaction
with the driver as possible. How we'll do this is still uncertain, but our initial
idea is to run a compute shader in each API that also requires memory
transfers between GPU and CPU to increase the amount of communication that needs
to be done and synchronized. One such task could be Gaussian Elimination where
forward-substitution is done on the GPU and backward-substitution on the CPU. It
should also be a trivial task to make identical shader code across the APIs
for a task such as this.

# Instructions

Compilation instructions using CMake

## Prerequisites

* git
* CMake 3.8.2 (or higher)
* A c++ compiler with c++14 compatability

## Compilation on Linux

* After cloning the repo, navigate into the created folder.
* Make a new folder named 'build'.
* Navigate into the build folder.
* From the terminal, execute the command `cmake ..` to execute cmake
* To compile, run `make`.
* The executable is found in `build/benchmark/src/` (relative to repo root) and should be named `benchmark`.

## Compilation on Windows (using Visual Studio 2017)

Visual Studio 2017 comes with integrated CMake support which should make this process
fairly simple.

* After cloning the repo, open Visual Studio and instead of opening a project, select Open Folder, and navigate to the newly cloned repo.
* You are now using VS2017 in a CMake environment. Some settings will have changed from what you may be used to. You should still be able to compile and run as usual. The runnable/debugable target should be called `benchmark`.

