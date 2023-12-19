add_library(${PROJECT_NAME} STATIC
  hashtable.c
  priorityqueue.c
  queue.c
  avltree.c
  functions.c
  memory.c
)

list(APPEND COMPILE_OPTS -O2)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${INCLUDE_PATH}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${COMPILE_OPTS}
)
