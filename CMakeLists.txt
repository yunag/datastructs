cmake_minimum_required(VERSION 3.5)
project(datastructs VERSION 0.0.1 LANGUAGES C)

add_library(${PROJECT_NAME} STATIC
  src/hashtable.c
  src/trie.c
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    include
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -ggdb>
)
