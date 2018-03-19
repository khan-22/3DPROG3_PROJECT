# Driver Overhead Benchmark [3DPROG3 PROJECT]
Our project in DV2551

Members: Karl Hansson, Filip Pentikäinen, Peyman Torabi

The aim of our project is to attempt to measure how significant the driver
overhead in OpenGL may be as compared to "equivalent" tasks in Vulkan. We want
to find how much it matters to do things optimally in either API, as well as
where in OpenGL you may not be able to get a straight answer. We'll be investigating
as much as we can afford with regards to rendering triangles.
(Initialization, Vertex Buffer creation, Shader pipeline creation, Rendering...)

[comment]: <> (The aim of our project is to measure the driver overhead in Vulkan, 
as compared with OpenGL to determine
how significant the difference may be during different stages of the program
(initialization, execution, clean up, etc...). Since Vulkan claims
to be low-overhead, we would expect to see that reflected in the data, assuming
our tests are accurately representing reality. )

We'd also hope to be able to draw
some conclusions around what parts of the program that the different APIs
have the most overhead. Since one of the strengths of Vulkan is that you have
greater control over the creation of pipelines, we would expect the initialization part to
take longer than the other APIs depending on how they choose to defer pipeline
creation, while execution might be lower overhead for Vulkan. You may call this
part of our _hypothesis_.

We aim to do this by essentially implementing the same program multiple times as
static libraries, one for each API, which can be executed from a single benchmark
executable that collects our desired metrics. Our main metric is time, but the
possibility to measure memory overhead also exists.

Since we want to measure driver overhead we want to try to have as much interaction
with the driver as possible. We'll be doing things such as creating thousands of vertex buffers
and filling them with data. The question asked being: "What do we think OpenGL did when we performed this command?
And how do we think they did it?". We'll also test how inefficient binding operations affect performance in either API.
For instance, imagine the case where we are going to render 400 triangles, where there are only 4 pipelines. To render 100 triangles
for each pipeline most efficiently would be to bind a pipeline, render 100 triangles, and repeat. To do it inefficiently would be to
bind a pipeline, render a triangle, bind _another_ pipeline, render _another_ triangle, and repeat. Which API does this affect the most?



# Test structure
* **Initialization**
  * **Window Creation**
    * Both API's will be using the same window library (glfw). OpenGL context creation is mostly done by the help of this library, meanwhile most Vulkan initialization is done by hand.
  * **Resource Creation** 
	* Create _N_ vertex buffers + data (in multiple different ways so that we have multiple points of comparison).
    * Create _2N_ shader modules with slightly differing source to avoid caching.
    * Create _N_ Pipelines with the help of the previous shader modules.
  * **Drawing**
    * (In each test we have the ability to split the work into multiple threads on Vulkan).
    * _"Single triangle test"_. Draw one triangle per pipeline (_N_ triangles). We expect the time we use a pipeline to be extra expensive due to the OpenGL deferring pipeline validation, so we want this to be a special data-point. 
    * _"Single pipeline test"_. Use one pipeline to draw many triangles (_N_ triangles). Like the pipeline validation, we'd expect to see the transfer of the vertex buffer to be deferred.
    * _"Optimal test"_. Draw _M_ triangles per pipeline (_N * M_ triangles). Bind pipelines optimally.
    * _"Non-optimal test"_. Draw _M_ triangles per pipeline (_N * M_ triangles). Bind pipelines non-optimally.
  * **Clean Up**
    * No real thorough tests here. Mostly just time the shutdown.

# Instructions

Compilation instructions using CMake

## Prerequisites

* <img src="https://git-scm.com/images/logos/logomark-orange@2x.png" alt="IMAGE ALT TEXT HERE" width="16" height="16" border="10" /> git
* <img src="https://www.macupdate.com/images/icons256/49301.png" alt="IMAGE ALT TEXT HERE" width="16" height="16" border="10" /> CMake 3.8.2 (or higher)
* A c++ compiler with c++14 compatability
* A 64-bit version of either Windows 10 or a Linux distro such as Ubuntu 16.04.

## Compilation on Linux (RECOMMENDED)

* After cloning the repo, navigate into the created folder.
* Make a new folder named 'build'.
* Navigate into the build folder.
* From the terminal, execute the command `cmake ..` to execute cmake
* To compile, run `make`.
* The executable is found in `build/benchmark/src/` (relative to repo root) and should be named `benchmark`.

*NOTE:* The output after running should be found in the `output/` folder in the repo root.

## Compilation on Windows (using Visual Studio 2017)

Visual Studio 2017 comes with integrated CMake support which should make this process
fairly simple.

Please note that you must enable `Visual C++ tools for CMake` in the Visual Studio
installer for the integrated support!

* After cloning the repo, open Visual Studio and instead of opening a project, select Open Folder, and navigate to the newly cloned repo.
* You are now using VS2017 in a CMake environment. Some settings will have changed from what you may be used to. You should still be able to compile and run as usual. The runnable/debugable target should be called `benchmark`. Remember to set the configuration to `x64-Release`.

*NOTE:* Since Visual Studio determines where your working directory and executables are placed, it has chosen to put them in a strange location. When opening the cmake project you will see it configure in the CMake window (down in the output panel). Look for the "Working directory" statement.
There you will find a path, such as `C:\Users\<USERNAME>\CMakeBuilds\b202b102-5c6f-d13a-888a-81fc15b858ca\build\x64-Release`, where the string of numbers and letters are unique to you. In it you will find the executable.
In order to see the results you MUST manually create an `output/` folder here. This is why we recommend running the program on Linux.

## Compilation on Windows (using CMake-GUI)

If you are using an earlier version of Visual Studio or something completely different,
you can use cmake-gui to generate a visual studio project instead.

It is recommended that you use the previous approach if at all possible.

* After cloning the repo, open cmake-gui and pick it as the "source directory".
* Create a folder named 'build' in the reposotiry root folder and select it as the "build destination".
* Click the 'Configure' button and choose the build system you want to use.
* Click generate.
* Once finished, if you chose visual studio you will now have a Visual Studio Solution and a number of projects available to you in the build folder. To compile and debug you simply open the solution file in visual studio and work as per usual.
* NOTE: AVOID changing settings directly in the solution or project files if they can instead be added into the CMake configurations.

*NOTE:* If the output is not found in the `output/` folder you may need to create one inside of the build folder you just created.
