project(CellSim)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(third_party)

add_executable(hello hello.cpp)

target_link_libraries(hello cellml)
target_include_directories(hello PRIVATE $<TARGET_PROPERTY:cellml,INTERFACE_INCLUDE_DIRECTORIES>)
