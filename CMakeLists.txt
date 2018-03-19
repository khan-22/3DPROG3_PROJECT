cmake_minimum_required(VERSION 3.8.2)

project(VulkanProjectBenchmark)

add_subdirectory(thirdpartycommon/glfw)

add_subdirectory(libtemplate)
add_subdirectory(libbenchvulkan)
add_subdirectory(libbenchopengl)
add_subdirectory(benchmark)
