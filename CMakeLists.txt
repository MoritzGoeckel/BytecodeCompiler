cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests src/Tests.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")