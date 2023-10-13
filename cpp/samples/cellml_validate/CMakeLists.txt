add_executable(cellml_validate cellml_validate.cpp)

target_link_libraries(cellml_validate
    PRIVATE
    cellml)
target_link_libraries(cellml_validate
    PRIVATE
    mongoose)

target_include_directories(cellml_validate PRIVATE $<TARGET_PROPERTY:cellml,INTERFACE_INCLUDE_DIRECTORIES>)
