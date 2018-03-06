set(TARGET_NAME benchmark)

add_executable(${TARGET_NAME}
  src/main.cpp
  src/result_collection.cpp
)

target_link_libraries(${TARGET_NAME}
  PRIVATE
    benchvulkan benchopengl
)

