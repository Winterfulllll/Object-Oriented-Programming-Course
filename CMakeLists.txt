cmake_minimum_required(VERSION 3.10)
project(MyLabs)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

if(DEFINED ENV{GITHUB_HEAD_REF})
  set(BRANCH_NAME $ENV{GITHUB_HEAD_REF})
else()
  set(BRANCH_NAME "lab1") # Default branch for local builds
  message(STATUS "Not running in GitHub Actions context. Building ${BRANCH_NAME}.")
endif()

if(EXISTS "solutions/${BRANCH_NAME}")
  add_executable(${BRANCH_NAME}_tests solutions/${BRANCH_NAME}/tests.cpp)
  target_link_libraries(${BRANCH_NAME}_tests PRIVATE gtest_main)
  target_include_directories(${BRANCH_NAME}_tests PRIVATE ${gtest_SOURCE_DIR}/include)
else()
  message(FATAL_ERROR "Error: Branch '${BRANCH_NAME}' not found in 'solutions/' directory.")
endif()
