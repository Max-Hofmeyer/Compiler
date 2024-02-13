# CMakeList.txt : CMake project for part1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (part1 "part1.cpp" "part1.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET part1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
