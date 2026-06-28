cmake_minimum_required(VERSION 3.28)
project(cppglfwinput)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# so clangd won't bother me
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(spdlog REQUIRED)
find_package(glfw3 REQUIRED)

if (NOT TARGET spdlog::spdlog)
  add_library(spdlog::spdlog ALIAS spdlog)
endif()
if (NOT TARGET glfw::glfw)
  add_library(glfw::glfw ALIAS glfw)
endif()

add_library(debug_utils INTERFACE)
target_link_libraries(debug_utils INTERFACE
  log
)
target_compile_definitions(debug_utils INTERFACE
  $<$<CONFIG:Debug>:SG_DEBUG=1>
  $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE>
  $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

add_library(utils INTERFACE)
target_include_directories(utils INTERFACE src/utils)
target_link_libraries(utils INTERFACE debug_utils log)

add_library(log INTERFACE
  src/utils/log.cpp
)
target_include_directories(log INTERFACE src/utils)
target_link_libraries(log INTERFACE spdlog::spdlog)
add_library(input STATIC
  src/input/input.hpp
  src/input/actionset.hpp
  src/input/input.cpp
  src/input/actionset.cpp
)
target_include_directories(input PUBLIC src/input)
target_link_libraries(input PUBLIC glfw::glfw utils debug_utils log)

add_executable(main src/main.cpp)
target_link_libraries(main
  PRIVATE
    utils
    log
    input
    glfw::glfw
  PUBLIC debug_utils
)
