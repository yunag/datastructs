cmake_minimum_required(VERSION 3.16)
project(datastructs VERSION 0.0.1 LANGUAGES C)
option(build_tests "Build tests." ON)

set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(COMPILE_OPTS 
  -pedantic 
  -Wall
  -Wextra 
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Disable secure warnings
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(COMPILE_OPTS)
endif()

add_subdirectory(src)
if(build_tests)
  enable_testing()
  add_subdirectory(tests)
endif()
