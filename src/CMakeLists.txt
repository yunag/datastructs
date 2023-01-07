add_library(${PROJECT_NAME} STATIC
  hashtable.c
  trie.c
  priorityqueue.c
  queue.c
  stack.c
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${INCLUDE_PATH}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -ggdb>
)
