cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
add_executable(runTests 
  Source/Tests/Tests.cpp
)

target_link_libraries(runTests 
  ${GTEST_LIBRARIES} 
  pthread
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -O3 -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O3 -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -Wno-unused-variable")