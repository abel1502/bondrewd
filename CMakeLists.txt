cmake_minimum_required(VERSION 3.20)

# set(CMAKE_CXX_COMPILER clang++-cl)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS false)

set(CXX_WARNING_FLAGS "\
	-Wall -Wno-multichar -Wno-c++98-compat-pedantic \
	-Wno-shadow-field-in-constructor -Wno-old-style-cast \
	-Wno-sign-conversion -Wno-extra-semi-stmt \
")
set(CXX_OPTIMIZE_1_FLAGS "-O2 -g")

set(CMAKE_EXE_LINKER_FLAGS "-m64")
set(CMAKE_CXX_FLAGS "${CXX_WARNING_FLAGS} -m64 -march=native ${CXX_OPTIMIZE_FLAGS}")


get_filename_component(LIB_ROOT "." ABSOLUTE)
file(GLOB_RECURSE LIB_SOURCES ${LIB_ROOT}/src/*.cpp)
file(GLOB_RECURSE LIB_HEADERS_PUBLIC ${LIB_ROOT}/include/*.hpp ${LIB_ROOT}/include/*.ipp)
file(GLOB_RECURSE LIB_HEADERS_PRIVATE ${LIB_ROOT}/src/*.hpp ${LIB_ROOT}/src/*.ipp)
set(LIB_HEADERS ${LIB_HEADERS_PUBLIC} ${LIB_HEADERS_PRIVATE})


project(bondrewd)


add_library(bondrewd-compiler STATIC ${LIB_SOURCES})
target_include_directories(bondrewd-compiler PUBLIC ${LIB_ROOT}/include)
target_include_directories(bondrewd-compiler PRIVATE ${LIB_ROOT}/src)  # TODO: Do I need this?

find_package(fmt CONFIG REQUIRED)
target_link_libraries(bondrewd-compiler PRIVATE fmt::fmt)
