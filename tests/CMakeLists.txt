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

list(APPEND Targets queue priorityqueue stack hashtable)
list(APPEND Sources queuetest.cpp pqtest.cpp stacktest.cpp htabletest.cpp)
foreach(target source IN ZIP_LISTS Targets Sources)
  add_executable(${target} ${source})
  target_link_libraries(${target}
    PRIVATE
      ${TEST_LIBS}
  )
  add_test(${target}Test ${target})
endforeach()
