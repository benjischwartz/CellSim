add_executable(hello hello.cpp)

target_link_libraries(hello 
    PRIVATE
    cellml)
target_link_libraries(hello
    PRIVATE
    mongoose)

target_include_directories(hello PRIVATE $<TARGET_PROPERTY:cellml,INTERFACE_INCLUDE_DIRECTORIES>)
