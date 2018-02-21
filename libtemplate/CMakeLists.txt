set(TARGET_NAME template)

add_library(${TARGET_NAME} STATIC
  src/bench_template.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    include/
)

target_compile_features(${TARGET_NAME}
  PUBLIC
    cxx_std_14
)