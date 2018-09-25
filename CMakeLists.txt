cmake_minimum_required(VERSION 3.3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(hpxapp1 CXX)
find_package(HPX REQUIRED)
add_hpx_executable(
    app1
    SOURCES prog.cpp
    COMPONENT_DEPENDENCIES iostreams
)