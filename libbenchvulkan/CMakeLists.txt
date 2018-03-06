set(TARGET_NAME benchvulkan)

find_package(Vulkan REQUIRED)

add_library(${TARGET_NAME} STATIC
  src/bench_vulkan.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    include/
)

target_link_libraries(${TARGET_NAME}
  PUBLIC
    template 
    Vulkan::Vulkan
    glfw
)