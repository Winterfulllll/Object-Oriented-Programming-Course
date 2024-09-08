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
  message(STATUS "Not running in GitHub Actions context. Building all labs.")
  set(BRANCH_NAME "")
endif()

add_subdirectory(solutions/${BRANCH_NAME})
