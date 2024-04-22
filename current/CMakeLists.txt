# CMakeList.txt : CMake project for part1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
project(current)

add_executable(current
	src/main.cpp
	src/scanner.cpp
	src/helpers/logger.cpp
	src/helpers/cliConfig.cpp
	src/parser/parser.cpp
	src/parser/ast.cpp
	src/codeGeneration/symbolTable.cpp
	src/codeGeneration/semanticAnalysis.cpp
	src/codeGeneration/jasminGenerator.cpp)

target_include_directories(current PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET current PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
