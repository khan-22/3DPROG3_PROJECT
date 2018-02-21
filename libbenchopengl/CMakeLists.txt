set(TARGET_NAME benchopengl)

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
)