file(GLOB_RECURSE CLI_SOURCES ${LIB_ROOT}/cli/*.cpp)

add_executable(bondrewd ${CLI_SOURCES})

target_link_libraries(bondrewd PUBLIC bondrewd-compiler)

set_target_properties(bondrewd PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${LIB_ROOT}/out"  # TODO: ${CMAKE_SOURCE_DIR}?
)

find_package(Boost 1.81.0 REQUIRED COMPONENTS program_options)
target_link_libraries(bondrewd PUBLIC Boost::program_options)
