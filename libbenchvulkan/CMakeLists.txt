set(TARGET_NAME benchvulkan)

add_library(${TARGET_NAME} 
  src/bench_vulkan.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    include/
)

target_link_libraries(${TARGET_NAME}
  PUBLIC
    template
)