set(TARGET_NAME benchmark)

add_executable(${TARGET_NAME}
  src/main.cpp
)

target_link_libraries(${TARGET_NAME}
  PRIVATE
    benchvulkan benchopengl
)

#set_target_properties(${TARGET_NAME} PROPERTIES
#	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
#)

FILE(COPY "${CMAKE_SOURCE_DIR}/res" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")