add_library(${PROJECT_NAME} STATIC
  hashtable.c
  trie.c
  priorityqueue.c
  queue.c
  stack.c
  avltree.c
  functions.c
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${INCLUDE_PATH}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${COMPILE_OPTS}
)
