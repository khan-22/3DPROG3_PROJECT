set(TARGET_NAME benchdx11dc)

add_library(${TARGET_NAME} STATIC
  src/bench_dx11dc.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    include/
)

target_link_libraries(${TARGET_NAME}
  PUBLIC
    template
)

