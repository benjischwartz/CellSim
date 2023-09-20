project(CellSim)

include(FetchContent)

FetchContent_Declare(
    libcellml
    GIT_REPOSITORY https://github.com/cellml/libcellml.git
    GIT_TAG origin/main
)

FetchContent_GetProperties(libcellml)

if (NOT libcellml_POPULATED)
    FetchContent_Populate(libcellml)
    add_subdirectory(${libcellml_SOURCE_DIR}
        ${libcellml_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
