cmake_minimum_required(VERSION 3.18.0)

# required for unit testing using CMake's ctest command
include(CTest)
enable_testing()

# required modules for our task
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckIncludeFiles)
include(ExternalProject)
# TO BE CREATED
include(cpp/third_party/libcellml.cmake)

# we add the files in the include directory to be included
# while compiling all the source code
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# we add the sub-directories that we want CMake to scan
add_subdirectory(include)
add_subdirectory(cpp/third_party)
add_subdirectory(src)
add_subdirectory(test)
