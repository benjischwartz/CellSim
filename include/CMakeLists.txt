project(CellSim)
# Fetch package from our system
find_package(libcellml REQUIRED)

add_library(cellsim
    INTERFACE # This library is header only (no source files)
    ${CELLSIM_INTERFACE_FILES})

set(CELLSIM_INTERFACE_FILES
    cellsim/cell_sim_util.h)

target_link_libraries(cellsim
    INTERFACE
    libcellml
)


# Whoever is linking against our library can include this directory directly
target_include_directories(cellsim
    INTERFACE
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INDCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

install(TARGETS cellsim EXPORT ${CMAKE_PROJECT_NAME} DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cellsim/cmake)

install(FILES ${CELLSIM_INTERFACE_FILES} DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/cellsim})
