cmake_minimum_required(VERSION 3.18.0)

project(CellSim)

# Executes CMakeLists.txt file inside subdirectory
add_subdirectory(cpp/third_party)
add_subdirectory(include)
add_subdirectory(src)

option(BUILD_EXAMPLES "Whether or not to build examples"
    ON)

set(CMAKE_CXX_STANDARD 17)

if (BUILD_EXAMPLES)
    message(STATUS "Building examples...")
    add_subdirectory(example)
endif()

install(FILES cmake/cellsim-config.cmake DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cellsim/cmake)

install(EXPORT ${CMAKE_PROJECT_NAME} DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cellsim/cmake)
