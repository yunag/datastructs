project(datastructs_test LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_LIBS gtest_main gmock_main datastructs)
set(TEST_COMPILE_OPTS)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # Disable secure warnings
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
  list(APPEND TEST_COMPILE_OPTS
    -fno-omit-frame-pointer
    -fno-common
    -Wall
    -Wextra
  )
else()
  unset(TEST_COMPILE_OPTS CACHE)
endif()

if(UNIX OR APPLE)
  list(APPEND TEST_LIBS -fsanitize=leak,address,undefined)
  list(APPEND TEST_COMPILE_OPTS -fsanitize=leak,address,undefined)
endif()

list(APPEND Targets queue priorityqueue hashtable avltree hashtablemock)
list(APPEND Sources queue.cpp priorityqueue.cpp hashtable.cpp avltree.cpp hashtablemock.cpp)
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
  set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)

  add_test(${target} ${target})
endforeach()
