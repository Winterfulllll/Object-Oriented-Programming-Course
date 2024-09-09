cmake_minimum_required(VERSION 3.10)
project(MyLabs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  TLS_VERIFY false
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(DEFINED ENV{GITHUB_HEAD_REF})
  set(BRANCH_NAME $ENV{GITHUB_HEAD_REF})
else()
  set(BRANCH_NAME "lab1") # Default branch for local builds
  message(STATUS "Not running in GitHub Actions context. Building ${BRANCH_NAME}.")
endif()

if(EXISTS "solutions/${BRANCH_NAME}")
  add_library(${BRANCH_NAME}_lib STATIC solutions/${BRANCH_NAME}/src/solution.cpp)

  add_executable(${BRANCH_NAME}_exe solutions/${BRANCH_NAME}/main.cpp)
  add_executable(${BRANCH_NAME}_tests solutions/${BRANCH_NAME}/tests/tests.cpp)

  target_link_libraries(${BRANCH_NAME}_exe PRIVATE ${BRANCH_NAME}_lib)
  target_link_libraries(${BRANCH_NAME}_tests PRIVATE gtest_main ${BRANCH_NAME}_lib)

  enable_testing()
  add_test(NAME AllTests COMMAND ${BRANCH_NAME}_tests)
else()
  message(FATAL_ERROR "Error: Branch '${BRANCH_NAME}' not found in 'solutions/' directory.")
endif()
