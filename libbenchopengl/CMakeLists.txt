set(TARGET_NAME benchopengl)

find_package(OpenGL REQUIRED)

# Disabling options for glbinding
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared instead of static libraries." FORCE)
set(OPTION_SELF_CONTAINED OFF CACHE BOOL "Create a self-contained install with all dependencies." FORCE)
set(OPTION_BUILD_TESTS OFF CACHE BOOL "Build tests." FORCE)
set(OPTION_BUILD_GPU_TESTS OFF CACHE BOOL "Build tests that require an OpenGL context." FORCE)
set(OPTION_BUILD_TOOLS OFF CACHE BOOL "Build tools." FORCE)

add_subdirectory(thirdparty/glbinding)

add_library(${TARGET_NAME} STATIC
  src/bench_opengl.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    include/
)

target_link_libraries(${TARGET_NAME}
  PUBLIC
    template
    glbinding::glbinding
    glfw
)