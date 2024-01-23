cmake_minimum_required(VERSION 3.16)
project(datastructs VERSION 0.0.1 LANGUAGES C)

option(build_tests "Build tests." ON)
option(build_examples "Build examples." ON)

set(DATASTRUCTS_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)

if(build_tests)
  enable_testing()
  add_subdirectory(tests)
endif()

# Don't build examples for MSVC
if(build_examples AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_subdirectory(examples)
endif()
