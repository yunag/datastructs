cmake_minimum_required(VERSION 3.16)
project(datastructs VERSION 0.0.1 LANGUAGES C)
option(build_tests "Build tests." ON)

set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(COMPILE_OPTS)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND COMPILE_OPTS
    -fno-omit-frame-pointer
    -fno-common
    -Wall
    -Wextra
    -pedantic
  )
endif()

add_subdirectory(src)
if(build_tests)
  enable_testing()
  add_subdirectory(tests)
endif()
