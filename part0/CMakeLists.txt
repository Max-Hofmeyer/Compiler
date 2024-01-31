cmake_minimum_required (VERSION 3.8)

project ("part0")

add_executable(part0
	src/main.cpp
	src/file1.cpp
	src/file2.cpp)

target_include_directories(part0 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)