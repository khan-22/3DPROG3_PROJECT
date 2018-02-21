set(TARGET_NAME benchmark)

add_executable(${TARGET_NAME}
  src/main.cpp
)

target_link_libraries(${TARGET_NAME}
  PRIVATE
    benchvulkan benchopengl benchdx11dc
)

