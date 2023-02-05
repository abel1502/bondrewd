cmake_minimum_required(VERSION 3.20)

# set(CMAKE_CXX_COMPILER clang++-cl)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS false)

set(CXX_WARNING_FLAGS "-Wall -Wextra -Wno-multichar")
set(CXX_OPTIMIZE_1_FLAGS "-O2 -g")

set(CMAKE_EXE_LINKER_FLAGS "-m64")
set(CMAKE_CXX_FLAGS "${CXX_WARNING_FLAGS} -m64 -march=native ${CXX_OPTIMIZE_FLAGS}")


get_filename_component(LIB_ROOT "." ABSOLUTE)
file(GLOB_RECURSE LIB_SOURCES ${LIB_ROOT}/src/*.cpp)


project(bondrewd)


add_library(bondrewd-compiler STATIC ${LIB_SOURCES})
target_include_directories(bondrewd-compiler PUBLIC ${LIB_ROOT}/include)
target_include_directories(bondrewd-compiler PRIVATE ${LIB_ROOT}/src)  # TODO: Do I need this?
target_link_libraries(bondrewd-compiler PUBLIC fmt)
