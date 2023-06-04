include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_LIBS gtest_main datastructs)
set(TEST_COMPILE_OPTS 
  -fno-omit-frame-pointer
  -fno-common
  -Wall
  -Wextra 
  -Werror
  -Wno-unused-label
  -Wno-unused-function
  -Wno-unused-parameter
)

if(UNIX AND APPLE)
  list(APPEND TEST_LIBS -fsanitize=leak,address,undefined)
  list(APPEND TEST_COMPILE_OPTS -fsanitize=leak,address,undefined)
endif()


list(APPEND Targets queue priorityqueue stack hashtable bst)
list(APPEND Sources queuetest.cpp pqtest.cpp stacktest.cpp htabletest.cpp bsttest.cpp)
foreach(target source IN ZIP_LISTS Targets Sources)
  add_executable(${target} ${source})
  target_link_libraries(${target}
    PRIVATE
      ${TEST_LIBS}
  )
  target_compile_options(${target}
    PRIVATE
      ${TEST_COMPILE_OPTS}
  )

  add_test(${target}Test ${target})
endforeach()
