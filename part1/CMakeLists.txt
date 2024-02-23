# CMakeList.txt : CMake project for part1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(part1
	src/main.cpp
	src/logger.cpp
	src/scanner.cpp
	src/cliConfig.cpp)


target_include_directories(part1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET part1 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
