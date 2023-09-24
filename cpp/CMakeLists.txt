project(CellSim)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

# third_party/libcellml/tests/test_utils.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")

add_subdirectory(third_party)
add_subdirectory(samples)

# Add targets here that will be targeted for CI.
add_custom_target(ci_targets)
add_dependencies(ci_targets hello)
add_dependencies(ci_targets mongoose_sample)

