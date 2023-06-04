cmake_minimum_required(VERSION 3.16)
project(datastructs VERSION 0.0.1 LANGUAGES C CXX)
option(build_tests "Build tests." ON)

set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(COMPILE_OPTS 
  -pedantic 
  -Wall
  -Wextra 
  -Wno-unused-label
  -Wno-unused-function
  -Wno-unused-parameter
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND COMPILE_OPTS -Wno-gnu-statement-expression)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(COMPILE_OPTS /Wall)
endif()

add_subdirectory(src)
if(build_tests)
  enable_testing()
  add_subdirectory(tests)
endif()
