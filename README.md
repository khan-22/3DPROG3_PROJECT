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

