project(datastructs_examples LANGUAGES C)

macro(add_example target file)
  add_executable(${target} ${file})
  target_link_libraries(${target}
    PRIVATE
      datastructs
  )
endmacro()

add_example(hashtable_example hashtable.c)
add_example(avltree_example avltree.c)
