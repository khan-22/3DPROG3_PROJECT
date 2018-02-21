set(TARGET_NAME template)

add_library(${TARGET_NAME} STATIC
  src/bench_template.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

