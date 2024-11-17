add_library(${PROJECT_NAME} STATIC
  hashtable.c
  priorityqueue.c
  queue.c
  avltree.c
  functions.c
  memory.c
)

set(DATASTRUCTS_COMPILE_OPTS)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND COMPILE_OPTS
    -fno-omit-frame-pointer
    -fno-common
    -Wall
    -Wextra
    -pedantic
  )
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${DATASTRUCTS_INCLUDE_PATH}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${DATASTRUCTS_COMPILE_OPTS}
)
