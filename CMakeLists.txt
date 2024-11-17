cmake_minimum_required(VERSION 3.16)
project(datastructs VERSION 0.0.1 LANGUAGES C)

option(DATASTRUCTS_BUILD_TESTS "Build tests." ON)
option(DATASTRUCTS_BUILD_EXAMPLES "Build examples." ON)

set(DATASTRUCTS_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)

if(DATASTRUCTS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(DATASTRUCTS_BUILD_EXAMPLES AND CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  # Currently examples supports only clang and gcc compilers

  add_subdirectory(examples)
endif()
