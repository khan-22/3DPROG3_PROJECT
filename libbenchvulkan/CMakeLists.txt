set(TARGET_NAME benchvulkan)

# set(SHADERC_SKIP_INSTALL ON CACHE BOOL "Skip installation" FORCE)
# set(SHADERC_SKIP_TESTS ON CACHE BOOL "Skip building tests" FORCE)

add_library(shaderc SHARED IMPORTED)
set_target_properties(shaderc PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shaderc/bin/linux/libshaderc_shared.so"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shaderc/include"
)

find_package(Vulkan REQUIRED)
# find_package(Shaderc REQUIRED)

add_library(${TARGET_NAME} STATIC
  src/bench_vulkan.cpp
)

target_include_directories(${TARGET_NAME}
  PUBLIC
    include/
  # PRIVATE
    # thirdparty/shaderc/include
)

target_link_libraries(${TARGET_NAME}
  PUBLIC
    template 
    Vulkan::Vulkan
    glfw
  PRIVATE
    shaderc
)

target_compile_definitions(${TARGET_NAME}
  PUBLIC
    VULKAN_HPP_NO_EXCEPTIONS
    VULKAN_HPP_NO_SMART_HANDLE
)